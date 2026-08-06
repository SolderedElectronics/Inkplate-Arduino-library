/**
 **************************************************
 * @file        Inkplate6COLOR_Image_Uploader.ino
 * @brief       Hosts a local web app for uploading an image and displaying it
 *              on Inkplate 6COLOR.
 *
 * @details     This example turns Inkplate 6COLOR into a small standalone
 *              Wi-Fi access point and HTTP web server that accepts an uploaded
 *              JPEG image from a browser and renders it on the e-paper display.
 *
 *              A client device connects directly to the Inkplate-hosted Wi-Fi
 *              network, opens the local upload page, selects an image, and
 *              sends it to the board. The uploaded JPEG is stored temporarily
 *              in RAM and then drawn from memory onto the display (not from SD
 *              storage). Because the board runs as a local access point and web
 *              server, Internet access is not required.
 *
 *              This workflow is useful for quick local image transfer without
 *              cloud services, external apps, or removable media. Because the
 *              upload buffer is RAM-based, practical image size is limited by
 *              available memory: the image is intended to be resized by the
 *              web app / workflow before display, but large uploads may still
 *              fail if memory usage exceeds what the board can hold, so smaller
 *              images are safer for reliable operation. Display mode is Inkplate
 *              6COLOR colour e-paper mode with full refreshes, which are slower
 *              than monochrome partial-update workflows on supported boards.
 *
 *              Expected output: startup instructions showing the SSID, password
 *              and local IP address, then the uploaded JPEG image after a
 *              successful browser upload; the browser serves a local upload page
 *              and preview endpoint; Serial shows upload progress, buffer
 *              allocation details and image transfer logs.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      phone / browser
 * - Serial:     115200 baud (optional for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Change the AP SSID/password in the sketch if needed.
 * 3) Upload the sketch to Inkplate 6COLOR.
 * 4) After boot, note the Wi-Fi SSID, password and local IP shown on the
 *    display.
 * 5) Connect a phone or computer to the Inkplate Wi-Fi network.
 * 6) Open a browser and navigate to the IP address shown on the display.
 * 7) Use the upload page to select and send a JPEG image; after the upload
 *    finishes, the image is rendered on the Inkplate display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025
 * @license     GNU GPL V3
 **************************************************/

// Ensure correct board is selected
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include <WiFi.h>           // WiFi support
#include <WebServer.h>      // Built-in web server
#include <HTTPClient.h>     // HTTP client for image upload
#include <Inkplate.h>       // Inkplate display library
#include "src/html.h"           // HTML page definitions (INDEX_HTML)

// Wi-Fi Access Point credentials
const char* ap_ssid     = "InkplateImage"; // SSID for AP mode
const char* ap_password = "inkplate";       // Password (min 8 chars)

// Initialize Inkplate 
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
      // Fallback capacity if header is missing or invalid (~600x488 pixels)
      imageBufCapacity = 600 * 448;
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
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print("Welcome to the Inkplate Image Uploader example.");
  display.setTextSize(1);
  display.setCursor(10, 50);
  display.print("Connect to WiFi:");
  display.setCursor(30, 70);
  display.print("SSID: " + String(ap_ssid));
  display.setCursor(30, 90);
  display.print("Password: " + String(ap_password));
  display.setCursor(10, 110);
  display.print("Then visit:");
  display.setCursor(10, 130);
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
