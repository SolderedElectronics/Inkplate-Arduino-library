/**
 **************************************************
 * @file        Inkplate2_Image_Uploader.ino
 * @brief       WiFi image uploader: host a web page in AP mode, accept a JPEG
 *              upload, serve a preview, and render the uploaded image on the
 *              Inkplate display.
 *
 * @details     This example turns Inkplate 2 into a simple standalone web app
 *              for uploading an image from a phone/PC. The ESP32 starts a WiFi
 *              Access Point (AP) and an HTTP server on port 80. A web page
 *              (served from src/html.h, which must provide INDEX_HTML) lets the
 *              user upload a JPEG file to the device. The uploaded JPEG is stored
 *              in a RAM buffer and can be previewed from the browser via /preview
 *              and /image.jpg.
 *
 *              After a successful upload, the sketch draws the JPEG directly
 *              from the RAM buffer onto the e-paper display using
 *              drawJpegFromBuffer(), then performs a full refresh. The sketch
 *              initializes the Inkplate display in its default mode (as
 *              configured by the library for Inkplate 2) and no partial updates
 *              are used.
 *
 *              The uploaded JPEG is stored fully in RAM, so large files may fail
 *              to allocate or may be truncated if Content-Length is missing or
 *              incorrect - the upload handler allocates a buffer based on the
 *              HTTP Content-Length header and falls back to a small default
 *              capacity if the header is absent. JPEG decoding and drawing can be
 *              slow for large images, so prefer reasonably sized uploads. This is
 *              a demo web server without authentication beyond the AP password,
 *              so avoid using it in untrusted environments.
 *
 *              Expected output: startup instructions (AP SSID/password + IP
 *              address) on the display, the upload page at "/" and preview page
 *              at "/preview" in the browser, upload progress and transmitted byte
 *              counts in the Serial Monitor, and the uploaded image drawn on the
 *              e-paper display after the upload.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Phone/PC with WiFi + web browser
 * - Serial:     115200 baud (optional, for upload logs)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Set ap_ssid / ap_password (minimum 8 characters for WPA2).
 * 3) Upload the sketch to Inkplate 2.
 * 4) On your phone/PC, connect to the WiFi AP shown on the display.
 * 5) Open a browser and navigate to the AP IP address shown on the display.
 * 6) Use the page to upload a JPEG image.
 * 7) The device stores the upload in RAM, serves a preview, and renders the
 *    image on the e-paper display after upload completes.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026
 * @license     GNU GPL V3
 **************************************************/

// Ensure correct board is selected
#if !defined(ARDUINO_INKPLATE2)
#error "Wrong board selection for this example, please select e-radionica Inkplate2 or Soldered Inkplate2 in the boards menu."
#endif

#include <WiFi.h>           // WiFi support
#include <WebServer.h>      // Built-in web server
#include <HTTPClient.h>     // HTTP client for image upload
#include <Inkplate.h>       // Inkplate display library
#include "src/html.h"           // HTML page definitions (INDEX_HTML)

// Wi-Fi Access Point credentials
const char* ap_ssid     = "InkplateImage"; // SSID for AP mode
const char* ap_password = "inkplate";       // Password (min 8 chars)

// Initialize Inkplate (3-bit grayscale mode by default)
Inkplate display;

// Create a web server object on port 80
WebServer server(80);

// Buffer to hold the latest uploaded image in RAM
uint8_t* imageBuf         = nullptr;  // pointer to JPEG data
size_t   imageLen         = 0;        // actual bytes received
size_t   imageBufCapacity = 0;        // allocated buffer size

// Flag to indicate a completed upload
bool imageUploaded = false;

// Serve the main HTML page when root is accessed
void handleIndex() {
  server.send_P(200, "text/html", INDEX_HTML);
}

// Handle file upload in three stages: START, WRITE, END
void handleUpload() {
  HTTPUpload& u = server.upload();

  if (u.status == UPLOAD_FILE_START) {
    // Beginning of upload: allocate buffer based on Content-Length header
    imageLen = 0;
    if (imageBuf) {
      free(imageBuf);
      imageBuf = nullptr;
    }
    imageBufCapacity = server.header("Content-Length").toInt();
    if (imageBufCapacity <= 0) {
      // Fallback capacity if header is missing or invalid (~212x104 pixels)
      imageBufCapacity = 212 * 104;
    }
    imageBuf = (uint8_t*) malloc(imageBufCapacity);
    Serial.printf("Upload start, buffer capacity = %u bytes\n",
                  (unsigned)imageBufCapacity);
  }
  else if (u.status == UPLOAD_FILE_WRITE) {
    // Middle of upload: append received chunk into buffer
    if (imageBuf && (imageLen + u.currentSize <= imageBufCapacity)) {
      memcpy(imageBuf + imageLen, u.buf, u.currentSize);
      imageLen += u.currentSize;
      Serial.printf("  received %u bytes (total %u)\n",
                    (unsigned)u.currentSize, (unsigned)imageLen);
    }
  }
  else if (u.status == UPLOAD_FILE_END) {
    // End of upload: mark flag to display the image
    Serial.printf("Upload complete, final size = %u bytes\n",
                  (unsigned)imageLen);
    imageUploaded = true;
  }
}

// After upload ends, reload the page so the user sees the result
void handleUploadComplete() {
  server.send(200, "text/html");
}

// Show a preview page with the last uploaded image
void handlePreview() {
  String html = "<html><body><h3>Preview:</h3>"
    "<img src=\"/image.jpg?ts=" + String(millis()) +
    "\" style=\"max-width:100%;\">"
    "</body></html>";
  server.send(200, "text/html", html);
}

// Serve the JPEG image bytes directly from RAM
void handleImage() {
  if (imageBuf && imageLen > 0) {
    // Send appropriate headers for JPEG
    server.sendHeader("Content-Type",   "image/jpeg");
    server.sendHeader("Content-Length", String(imageLen));
    server.send(200, "image/jpeg", "");  // end headers

    // Write image data to client
    WiFiClient client = server.client();
    size_t sent = client.write(imageBuf, imageLen);
    Serial.printf("Sent %u of %u bytes\n", sent, (unsigned)imageLen);
  }
  else {
    // No image available yet
    server.send(404, "text/plain", "No image uploaded");
  }
}

// Display the buffered image on the e-ink display
void showImageBuffer() {
  if (!imageBuf || imageLen == 0) return;

  display.clearDisplay();                     // clear existing content
  // Draw JPEG from RAM: full-screen, no dithering
  display.image.drawJpegFromBuffer(imageBuf, imageLen, 0, 0, true, false);
  display.display();                          // push to panel
}

// Helper: calculate scaled dimensions by the larger original side
void getScaledByLargerSide(int origW, int origH, int maxW, int maxH, int &outW, int &outH) {
  float ratio = (origW >= origH) ? (float(maxW) / origW) : (float(maxH) / origH);
  outW = round(origW * ratio);
  outH = round(origH * ratio);
}

void setup() {
  Serial.begin(115200);             // initialize serial for debug

  // Initialize the display
  display.begin();
  display.clearDisplay();

  // Start Wi-Fi in AP+STA mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP: "); Serial.println(apIP);

  // Show instructions on the e-ink screen
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Welcome to the Inkplate Image Uploader example.");
  display.setTextSize(1);
  display.setCursor(0,20);
  display.print("Connect to WiFi:");
  display.setCursor(10, 30);
  display.print("SSID: " + String(ap_ssid));
  display.setCursor(10, 40);
  display.print("Password: " + String(ap_password));
  display.setCursor(0,50);
  display.print("Then visit:");
  display.print(apIP.toString());
  display.display();

  // Define web routes and handlers
  server.on("/",            HTTP_GET,  handleIndex);
  server.on("/upload",      HTTP_POST, handleIndex, handleUpload);
  server.on("/preview",     HTTP_GET,  handlePreview);
  server.on("/image.jpg",   HTTP_GET,  handleImage);

  server.begin();             // start the HTTP server
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();      // handle incoming HTTP requests

  // If an image was uploaded, display it once
  if (imageUploaded) {
    imageUploaded = false;    // reset the flag
    delay(100);               // small delay to finish POST
    showImageBuffer();        // render image on e-ink
  }
}
