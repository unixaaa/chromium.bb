// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_SKCANVAS_VIDEO_RENDERER_H_
#define MEDIA_BLINK_SKCANVAS_VIDEO_RENDERER_H_

#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "media/base/media_export.h"
#include "media/base/video_rotation.h"
#include "media/filters/context_3d.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkXfermode.h"
#include "ui/gfx/geometry/rect.h"

class SkCanvas;

namespace media {

class VideoFrame;
class VideoImageGenerator;

// Handles rendering of VideoFrames to SkCanvases, doing any necessary YUV
// conversion and caching of resulting RGB bitmaps.
class MEDIA_EXPORT SkCanvasVideoRenderer {
 public:
  SkCanvasVideoRenderer();
  ~SkCanvasVideoRenderer();

  // Paints |video_frame| on |canvas|, scaling and rotating the result to fit
  // dimensions specified by |dest_rect|.
  // If the format of |video_frame| is VideoFrame::NATIVE_TEXTURE, |context_3d|
  // must be provided.
  //
  // Black will be painted on |canvas| if |video_frame| is null.
  void Paint(const scoped_refptr<VideoFrame>& video_frame,
             SkCanvas* canvas,
             const gfx::RectF& dest_rect,
             uint8 alpha,
             SkXfermode::Mode mode,
             VideoRotation video_rotation,
             const Context3D& context_3d);

  // Copy |video_frame| on |canvas|.
  // If the format of |video_frame| is VideoFrame::NATIVE_TEXTURE, |context_3d|
  // must be provided.
  void Copy(const scoped_refptr<VideoFrame>& video_frame,
            SkCanvas* canvas,
            const Context3D& context_3d);

  // Convert the contents of |video_frame| to raw RGB pixels. |rgb_pixels|
  // should point into a buffer large enough to hold as many 32 bit RGBA pixels
  // as are in the visible_rect() area of the frame.
  static void ConvertVideoFrameToRGBPixels(
      const scoped_refptr<media::VideoFrame>& video_frame,
      void* rgb_pixels,
      size_t row_bytes);

  // Copy the contents of texture of |video_frame| to texture |texture|.
  // |level|, |internal_format|, |type| specify target texture |texture|.
  // The format of |video_frame| must be VideoFrame::NATIVE_TEXTURE.
  static void CopyVideoFrameTextureToGLTexture(gpu::gles2::GLES2Interface* gl,
                                               VideoFrame* video_frame,
                                               unsigned int texture,
                                               unsigned int internal_format,
                                               unsigned int type,
                                               bool premultiply_alpha,
                                               bool flip_y);

 private:
  void ResetLastFrame();
  void ResetAcceleratedLastFrame();

  // An RGB bitmap and corresponding timestamp of the previously converted
  // video frame data by software color space conversion.
  SkBitmap last_frame_;
  base::TimeDelta last_frame_timestamp_;
  // If |last_frame_| is not used for a while, it's deleted to save memory.
  base::DelayTimer<SkCanvasVideoRenderer> frame_deleting_timer_;

  // This is a hardware accelerated copy of the frame generated by
  // |accelerated_generator_|.
  // It's used when |canvas| parameter in Paint() is Ganesh canvas.
  // Note: all GrContext in SkCanvas instances are same.
  SkBitmap accelerated_last_frame_;
  VideoImageGenerator* accelerated_generator_;
  base::TimeDelta accelerated_last_frame_timestamp_;
  base::DelayTimer<SkCanvasVideoRenderer> accelerated_frame_deleting_timer_;

  DISALLOW_COPY_AND_ASSIGN(SkCanvasVideoRenderer);
};

}  // namespace media

#endif  // MEDIA_BLINK_SKCANVAS_VIDEO_RENDERER_H_