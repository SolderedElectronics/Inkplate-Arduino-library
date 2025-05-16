#ifndef HTML_H
#define HTML_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1.0">
  <title>Inkplate Image Uploader</title>
  <style>
    :root {
      --primary: #5e2a8e;
      --accent: #00b8d4;
      --light: #f5f7fa;
      --dark: #333;
    }
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      font-family: sans-serif;
    }
    body {
      background: var(--light);
      color: var(--dark);
      padding: 1rem;
      text-align: center;
    }
    .upload-section {
      background: #fff;
      padding: 1rem;
      border-radius: 8px;
      max-width: 400px;
      width: 100%;
      margin: 2rem auto;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
    }
    .file-picker-buttons {
      display: flex;
      flex-wrap: wrap;
      gap: .5rem;
      justify-content: center;
      margin-bottom: 1rem;
    }
    .file-picker-buttons .upload-btn {
      flex: 1 1 45%;
      min-width: 120px;
    }
    #btn-upload {
      width: 100%;
      margin-top: .5rem;
    }
    .upload-btn {
      background: var(--accent);
      color: #fff;
      padding: .5rem 1rem;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    .upload-btn:disabled {
      opacity: .5;
      cursor: not-allowed;
    }
    .status {
      margin-top: 1rem;
      font-weight: bold;
      min-height: 1.2em;
    }
    /* Hide the real file inputs */
    input[type=file] {
      display: none;
    }
  </style>
</head>
<body>
  <div class="upload-section">
    <h2>Inkplate Image Uploader</h2>
    <br>
    <p>Using this app you can easily upload pictures to your Inkplate board. Take a new photo (like a selfie) or browse from your gallery - then click upload to show the image on your Inkplate 10!</p>
    <br>
    <form id="uploadForm" method="POST" action="/upload" enctype="multipart/form-data">
      <div class="file-picker-buttons">
        <!-- Hidden file inputs -->
        <input type="file" accept="image/*" capture="environment" id="camera-input" name="image">
        <input type="file" accept="image/*" id="gallery-input" name="image">
        <!-- Visible pickers -->
        <button type="button" class="upload-btn" id="btn-camera">Take a Photo</button>
        <button type="button" class="upload-btn" id="btn-gallery">Choose from Gallery</button>
      </div>
      <button type="submit" class="upload-btn" id="btn-upload" disabled>Upload</button>
    </form>

    <!-- Status message -->
    <div id="statusMessage" class="status" aria-live="polite"></div>
  </div>

  <script>
    document.addEventListener('DOMContentLoaded', () => {
      const CANVAS_W = 1200;
      const CANVAS_H = 825;

      const form          = document.getElementById('uploadForm');
      const camInput      = document.getElementById('camera-input');
      const galInput      = document.getElementById('gallery-input');
      const btnCam        = document.getElementById('btn-camera');
      const btnGal        = document.getElementById('btn-gallery');
      const uploadBtn     = document.getElementById('btn-upload');
      const statusMessage = document.getElementById('statusMessage');

      // initial state
      uploadBtn.disabled = true;
      statusMessage.textContent = '';

      // wire picker buttons
      btnCam.addEventListener('click', () => camInput.click());
      btnGal.addEventListener('click', () => galInput.click());

      // enable upload when a file is selected
      [camInput, galInput].forEach(input => {
        input.addEventListener('change', () => {
          const file = camInput.files[0] || galInput.files[0];
          if (file) {
            statusMessage.textContent = 'Image ready to upload';
            uploadBtn.disabled = false;
          }
        });
      });

      form.addEventListener('submit', function(e) {
        e.preventDefault();
        const file = camInput.files[0] || galInput.files[0];
        if (!file) return;

        // disable controls during upload
        uploadBtn.disabled = true;
        btnCam.disabled    = true;
        btnGal.disabled    = true;
        statusMessage.textContent = 'Uploading...';

        // process image
        const img = new Image();
        img.onload = () => {
          // calculate scale to fit entire image into 1200x825
          const scale = Math.min(
            CANVAS_W / img.width,
            CANVAS_H / img.height,
            1 // don't scale up
          );
          const drawW = Math.round(img.width * scale);
          const drawH = Math.round(img.height * scale);
          const offsetX = Math.round((CANVAS_W - drawW) / 2);
          const offsetY = Math.round((CANVAS_H - drawH) / 2);

          // prepare canvas
          const canvas = document.createElement('canvas');
          canvas.width  = CANVAS_W;
          canvas.height = CANVAS_H;
          const ctx = canvas.getContext('2d');
          
          // clear canvas (optional)
          ctx.clearRect(0, 0, CANVAS_W, CANVAS_H);

          // draw the scaled, centered image
          ctx.drawImage(img, offsetX, offsetY, drawW, drawH);

          // convert to JPEG and upload
          canvas.toBlob(blob => {
            const formData = new FormData();
            const name = file.name.replace(/\.[^/.]+$/, '') + '.jpg';
            formData.append('image', blob, name);

            const xhr = new XMLHttpRequest();
            xhr.open('POST', '/upload', true);
            xhr.onload = () => {
              statusMessage.textContent = 'Image uploaded';
              // reset for next upload
              camInput.value = '';
              galInput.value = '';
              btnCam.disabled = false;
              btnGal.disabled = false;
            };
            xhr.onerror = () => {
              statusMessage.textContent = 'Upload failed';
              btnCam.disabled = false;
              btnGal.disabled = false;
            };
            xhr.send(formData);
          }, 'image/jpeg', 0.92);

          URL.revokeObjectURL(img.src);
        };
        img.onerror = () => {
          statusMessage.textContent = 'Selected file is not a valid image.';
        };
        img.src = URL.createObjectURL(file);
      });
    });
  </script>
</body>
</html>
)rawliteral";

#endif