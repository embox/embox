#!/usr/bin/env bash

ROOT_DIR=.
BASE_DIR=$ROOT_DIR
QT_SCRIPTS_DIR=$ROOT_DIR/scripts/continuous/qt

TIMEOUT=30
CAPTURED_VIDEO=$QT_SCRIPTS_DIR/captured.mpeg
GOLD_FRAMES_DIR=$QT_SCRIPTS_DIR/gold_moveblocks_frames
VIDEO_FRAME_MATCH=99 # Matching of some video frame to a gold frame in percents
LOG_FILE=$QT_SCRIPTS_DIR/out.log

# Start grabbing video from display :QT_VNC_DISPLAY_NR and save it
# to CAPTURED_VIDEO
timeout $TIMEOUT ffmpeg -draw_mouse 0 -video_size 1024x768 -framerate 25 \
	-f x11grab -i :$QT_VNC_DISPLAY_NR $CAPTURED_VIDEO &
# Start xvncviewer on display :QT_VNC_DISPLAY_NR
timeout $TIMEOUT xvncviewer -geometry 1024x768 10.0.2.16:5900

# Now search for the gold frames within captured video.
# If some of the gold frames are not found then exit with failure.
final_ret=0
for gold_frame in $GOLD_FRAMES_DIR/*; do
	echo "> Comparing captured video with ${gold_frame}"

	# Here we search within all captured video for the frame ${gold_frame}
	# by calculating the difference between current frame and ${gold_frame}.
	# The resulting image can differ from black frame at most
	# than (100 - VIDEO_FRAME_MATCH) percents of pixel.
	ffmpeg -i $CAPTURED_VIDEO -loop 1 -i ${gold_frame} -an \
		-filter_complex "blend=difference:shortest=1,blackframe=$VIDEO_FRAME_MATCH:1" \
		-f null - 2> $LOG_FILE

	# We acquired in LOG_FILE all matched frames. Now chech if there are
	# at least one matched frame.
	grep -q -i "frame:.*pblack:$VIDEO_FRAME_MATCH" $LOG_FILE
	ret=$?
	if [ $ret -ne 0 ]; then
		echo "Error: Gold frame ${gold_frame} was not found in $CAPTURED_VIDEO"
		final_ret=1
		break
	fi
done

rm -f $LOG_FILE
rm -f $CAPTURED_VIDEO
exit $final_ret
