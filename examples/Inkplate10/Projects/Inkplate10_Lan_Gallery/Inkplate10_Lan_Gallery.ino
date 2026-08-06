/**
 **************************************************
 * @file        Inkplate10_Lan_Gallery.ino
 * @brief       Hosts a LAN web page to upload images to SD, then displays the
 *              uploaded images randomly on Inkplate 10 in 3-bit grayscale.
 *
 * @details     This example turns Inkplate 10 into a local network (LAN) image
 *              gallery. The ESP32 connects to your WiFi and runs an async web
 *              server that allows multiple clients to upload image files from a
 *              browser. Uploaded files are written to the on-board microSD card
 *              and the sketch periodically selects a random image from the SD
 *              root directory and renders it on the e-paper display.
 *
 *              Supported formats are BMP and JPEG (JPG/JPEG). The sketch scans
 *              the SD card for matching file extensions, builds a circular
 *              linked list of filenames, and randomly selects an entry for
 *              display. Basic image dimension detection is implemented for BMP
 *              and JPEG to help with centering; if size detection fails, a
 *              fallback size is used. Images are drawn using Inkplate's
 *              image.draw() helper and a full refresh is performed.
 *
 *              The display runs in 3-bit grayscale mode (INKPLATE_3BIT), which
 *              is suitable for photos but does not support partial updates.
 *              This sketch is intended for continuous operation (no deep
 *              sleep); images rotate based on a configurable interval, and the
 *              image list is rebuilt after each upload.
 *
 *              SD card access is shared between the web server upload handler
 *              and the display loop, so this example uses a mutex to serialize
 *              SD reads/writes. Only files in the SD root directory are scanned;
 *              large SD card directories may slow down rebuildImageList()
 *              because it re-scans. JPEG/BMP decoding and buffering consume RAM,
 *              so very large images may fail to decode or draw. If mDNS ".local"
 *              addressing does not work on your network/device, use the
 *              Inkplate's IP address shown in Serial Monitor.
 *
 *              Expected output: the e-paper display shows a randomly chosen
 *              image from the SD card, centered, with a small footer label
 *              rendered on top; the Serial Monitor prints WiFi connection
 *              progress, SD scan results, detected image dimensions and
 *              upload/write diagnostics. After an upload completes, the new
 *              images become eligible for rotation.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable, microSD card (FAT32 formatted)
 * - Extra:      WiFi network + a phone/PC with a web browser on the same LAN
 * - Libraries:  AsyncTCP, ESPAsyncWebServer (Arduino Library Manager)
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Format a microSD card as FAT32 and insert it into Inkplate 10.
 * 3) Install AsyncTCP and ESPAsyncWebServer from Arduino Library Manager.
 * 4) Edit ssid/password and (optionally) IMAGE_CHANGE_INTERVAL (milliseconds,
 *    controls rotation timing).
 * 5) Upload the sketch and open Serial Monitor at 115200 baud.
 * 6) From a device on the same network, open the gallery page (mDNS hostname
 *    such as "langallery.local" if available, otherwise use the printed IP).
 * 7) Upload BMP/JPG images; after each upload the list is rebuilt and a random
 *    image is shown. Images will also rotate automatically over time.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2020
 * @license     GNU GPL V3
 **************************************************/

// Ensure corect board is selected
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Select 'Soldered Inkplate10' in the boards menu."
#endif

#include "Inkplate.h" //Inkplate library

// Initialize Inkplate (3-bit grayscale mode)
Inkplate display(INKPLATE_3BIT);


// WiFi network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Image rotation/change inteerval (milliseconds)
#define IMAGE_CHANGE_INTERVAL 30000UL

// Linked list structure for storing image filepaths
struct Node { 
    char* path; 
    int id; 
    Node* next; 
};

Node* head = nullptr;   // Pointer to the first node in circular list 
int nodeCount = 0; // Total number of images found

SemaphoreHandle_t sdMutex;  // Mutex for SD card access synchronization
SdFile* currentUploadFile = nullptr;    // File object for current upload
volatile bool uploadComplete = false;   //Flag to indicate upload completion

// Forward declaration
void setupWebServer();

// Add a new image node to the linked list
void addNode(const char* p, int id) {
  Node* n = (Node*)malloc(sizeof(Node));
  if (!n) return;
  n->path = strdup(p);
  n->id = id;
  if (!head) {
    head = n;
    n->next = head;
  } else {
    Node* t = head;
    while (t->next != head) t = t->next;
    t->next = n;
    n->next = head;
  }
  nodeCount++;
}

// Build a list of all image files found on the SD card
bool buildImageList() {
  Serial.println("Building image list...");
  if (!display.sdCardInit()) {
    display.clearDisplay();
    display.print("SD Card init failed!");
    display.display();
    return false;
  }
  SdFile root;
  if (!root.open("/")) {
    Serial.println("Failed to open root directory!");
    return false;
  }

  // Free any existing list before rebuilding
  if (head) {
    Node* current = head;
    Node* first = head;
    do {
      Node* next = current->next;
      free(current->path);
      free(current);
      current = next;
    } while (current != first);
    head = nullptr;
  }
  nodeCount = 0;

// Iterate through SD card root directory and find image files
  SdFile e;
  while (e.openNext(&root, O_RDONLY)) {
    char name[64];
    e.getName(name, sizeof(name));
    String nm(name);
    nm.toLowerCase();
    if (nm.endsWith(".bmp") || nm.endsWith(".jpg") || nm.endsWith(".jpeg")) {
      Serial.printf("Found image: %s\n", name);
      addNode(name, nodeCount);
    }
    e.close();
  }
  root.close();
  Serial.printf("Total images found: %d\n", nodeCount);
  return nodeCount > 0;
}

// Pick a random image node from the list
Node* pickRandomNode() {
  if (!head) return nullptr;
  int id = random(nodeCount);
  Node* t = head;
  while (t->id != id) t = t->next;
  return t;
}

//read BMP image dimensions directly from SD card
static bool readBmpSize_SdFat(const char* path, int &w, int &h) {
  w = h = 0;
  SdFile f;
  if (!f.open(path, O_RDONLY)) return false;

  uint8_t hdr[26];
  int n = f.read(hdr, sizeof(hdr));
  f.close();
  if (n < 26) return false;
  if (hdr[0] != 'B' || hdr[1] != 'M') return false;

  int32_t bw = (int32_t)hdr[18] | ((int32_t)hdr[19] << 8) | ((int32_t)hdr[20] << 16) | ((int32_t)hdr[21] << 24);
  int32_t bh = (int32_t)hdr[22] | ((int32_t)hdr[23] << 8) | ((int32_t)hdr[24] << 16) | ((int32_t)hdr[25] << 24);
  if (bw <= 0 || bh == 0) return false;
  w = (int)bw;
  h = (int)((bh < 0) ? -bh : bh);
  return true;
}

// Read JPEG image dimensions directly from SD card
static bool readJpegSize_SdFat(const char* path, int &w, int &h) {
  w = h = 0;
  SdFile f;
  if (!f.open(path, O_RDONLY)) return false;

  uint8_t b[2];
  if (f.read(b, 2) != 2 || b[0] != 0xFF || b[1] != 0xD8) { f.close(); return false; }

  const size_t SKIP_BUF = 64;
  uint8_t skipBuf[SKIP_BUF];

  auto readU8 = [&](uint8_t &out)->bool { return f.read(&out, 1) == 1; };
  auto readU16 = [&](uint16_t &out)->bool {
    uint8_t hi, lo;
    if (f.read(&hi,1) != 1 || f.read(&lo,1) != 1) return false;
    out = ((uint16_t)hi << 8) | lo;
    return true;
  };
  auto skipN = [&](uint32_t n)->bool {
    while (n > 0) {
      size_t chunk = (n > SKIP_BUF) ? SKIP_BUF : n;
      int r = f.read(skipBuf, chunk);
      if (r != (int)chunk) return false;
      n -= chunk;
    }
    return true;
  };

  // Parse JPEG markers until SOF segment found
  while (true) {
    uint8_t markerPrefix;
    do {
      if (!readU8(markerPrefix)) { f.close(); return false; }
    } while (markerPrefix != 0xFF);

    uint8_t marker;
    do {
      if (!readU8(marker)) { f.close(); return false; }
    } while (marker == 0xFF);

    if (marker == 0xD9) break;
    if (marker == 0xD8 || marker == 0x01) continue;

    uint16_t segLen;
    if (!readU16(segLen)) { f.close(); return false; }
    if (segLen < 2) { f.close(); return false; }

    bool isSOF =
      (marker >= 0xC0 && marker <= 0xC3) ||
      (marker >= 0xC5 && marker <= 0xC7) ||
      (marker >= 0xC9 && marker <= 0xCB) ||
      (marker >= 0xCD && marker <= 0xCF);

    if (isSOF) {
      uint8_t P, Hh, Hl, Wh, Wl;
      if (!readU8(P) || !readU8(Hh) || !readU8(Hl) || !readU8(Wh) || !readU8(Wl)) { f.close(); return false; }
      h = ((int)Hh << 8) | Hl;
      w = ((int)Wh << 8) | Wl;
      f.close();
      return (w > 0 && h > 0);
    } else {
      uint32_t toSkip = (uint32_t)segLen - 2;
      if (!skipN(toSkip)) { f.close(); return false; }
    }
  }

  f.close();
  return false;
}

// Detect image type and get its width and height
static bool getImageWH_SdFat(const char* path, int &w, int &h) {
  const char *ext = strrchr(path, '.');
  if (ext) {
    char e1 = tolower(*(ext+1));
    char e2 = tolower(*(ext+2));
    char e3 = tolower(*(ext+3));
    if ((e1=='b' && e2=='m' && e3=='p')) {
      if (readBmpSize_SdFat(path, w, h)) return true;
    } else if ((e1=='j' && e2=='p' && e3=='g') ||
               (e1=='j' && e2=='p' && e3=='e')) {
      if (readJpegSize_SdFat(path, w, h)) return true;
    }
  }
  // Fallback default size if detection fails
  w = 800;
  h = 600;
  return false;
}

// Display selected image file on the e-ink screen
void showImage(const char* path) {
  Serial.printf("Displaying: %s\n", path);
  display.clearDisplay();
  if(!display.sdCardInit()){
    display.clearDisplay();
    display.print("SD Card Init() failed!");
    display.display();
  }

  int imgW = 0, imgH = 0;
  bool okSize = getImageWH_SdFat(path, imgW, imgH);
  Serial.printf("Image size detected: %dx%d (ok=%s)\n", imgW, imgH, okSize ? "yes" : "no");

  const int dispW = display.width();
  const int dispH = display.height();

  int drawW = imgW;
  int drawH = imgH;

  int x = (dispW - drawW) / 2;
  int y = (dispH - drawH) / 2;

  Serial.printf("Draw at x=%d, y=%d (disp=%dx%d)\n", x, y, dispW, dispH);

  // Draw image fron SD card
  if (!display.image.draw(path, x, y, 3)) {
    display.setTextSize(2);
    display.setCursor(100, 300);
    display.println("Image load failed!");
  }

  // Overlay footer text (white on black background)
  const char* overlayText = "Inkplate LAN Gallery on langallery.local";
  display.setTextSize(1);

  int16_t textW = strlen(overlayText) * 6;
  int16_t textH = 10;
  int16_t padding = 6;

  int16_t boxX = display.width() - textW - padding * 2;
  int16_t boxY = display.height() - textH - padding * 2;
  int16_t boxW = textW + padding * 2;
  int16_t boxH = textH + padding * 2;

  // Black rectangle behind text
  display.fillRect(boxX, boxY, boxW, boxH, 0);

  // White text on top
  display.setTextColor(7);
  display.setCursor(boxX + padding, boxY + textH - 2);
  display.print(overlayText);

  display.display();
}

// Start receiving file data for upload
void startFileUpload(const char* filename) {
  if (xSemaphoreTake(sdMutex, portMAX_DELAY)) {
    display.sdCardInit();
    if (currentUploadFile) {
      currentUploadFile->close();
      delete currentUploadFile;
    }
    currentUploadFile = new SdFile();
    bool opened = currentUploadFile->open(filename, O_WRITE | O_CREAT | O_TRUNC);
    Serial.printf("Opening %s: %s\n", filename, opened ? "SUCCESS" : "FAILED");
    xSemaphoreGive(sdMutex);
  }
}

// Write incoming file data chunks to SD card
void writeFileData(uint8_t* data, size_t len) {
  if (xSemaphoreTake(sdMutex, portMAX_DELAY)) {
    if (currentUploadFile && currentUploadFile->isOpen()) {
      size_t written = currentUploadFile->write(data, len);
      Serial.printf("Wrote %d/%d bytes\n", written, len);
    } else {
      Serial.println("ERROR: File not open!");
    }
    xSemaphoreGive(sdMutex);
  }
}

// Finish file upload and close the file
void finishFileUpload() {
  if (xSemaphoreTake(sdMutex, portMAX_DELAY)) {
    if (currentUploadFile) {
      if (currentUploadFile->isOpen()) {
        currentUploadFile->sync();
        currentUploadFile->close();
        Serial.println("File closed and synced");
      }
      delete currentUploadFile;
      currentUploadFile = nullptr;
    }
    uploadComplete = true;
    xSemaphoreGive(sdMutex);
  }
}

void setup() {
  Serial.begin(115200);
  display.begin();
  display.setTextColor(BLACK);
  randomSeed(analogRead(0));
  sdMutex = xSemaphoreCreateMutex();

  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 0);
  display.println("Connecting Wi-Fi...");
  display.display();

  // Connect to existing WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected! IP: %s\n", WiFi.localIP().toString().c_str());

  // Start local web server fro uploads
  setupWebServer();

  // Build image list and show a random one at startup
  if (buildImageList()) {
    Node* n = pickRandomNode();
    if (n) showImage(n->path);
  } else {
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(100, 300);
    display.println("No images found on SD!");
    display.display();
  }
}

unsigned long lastImageChange = 0; // track last displayed image time

void loop() {
  if (uploadComplete) {
    uploadComplete = false;
    Serial.println("Upload complete, rebuilding list...");
    delay(100);
    if (xSemaphoreTake(sdMutex, portMAX_DELAY)) {
      if (buildImageList()) {
        Serial.printf("Found %d images\n", nodeCount);
        Node* n = pickRandomNode();
        if (n) {
          Serial.printf("Picked image: %s\n", n->path);
          showImage(n->path);
        }
      }
      xSemaphoreGive(sdMutex);
    }
    lastImageChange = millis();
  }

  // Automatically change image after the set interval
  if (millis() - lastImageChange >= IMAGE_CHANGE_INTERVAL) {
    if (xSemaphoreTake(sdMutex, portMAX_DELAY)) {
      if (buildImageList()) {
        Node* n = pickRandomNode();
        if (n) showImage(n->path);
      }
      xSemaphoreGive(sdMutex);
    }
    lastImageChange = millis();
  }

  delay(10); // small delay for task scheduling
}
