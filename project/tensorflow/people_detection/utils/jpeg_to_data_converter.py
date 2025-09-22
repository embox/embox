
import cv2
import numpy as np

# Load your original image
img = cv2.imread('testdata/person.jpg', cv2.IMREAD_GRAYSCALE)
print(f"Original size: {img.shape}")

# Resize to exact model input
img = cv2.resize(img, (96, 96))
print(f"After resize: {img.shape}")
print(f"Pixel range: {img.min()} to {img.max()}")

# Convert to INT8 (critical step)
# CORRECT - Subtract first, then cast to int8
img_int8 = (img.astype(np.int16) - 128).astype(np.int8)

print(f"INT8 range: {img_int8.min()} to {img_int8.max()}")

# Save for verification
cv2.imwrite('processed_96x96.png', img)  # Visual check
img_int8.tofile('tensor_debug.raw')  # For C array generation
