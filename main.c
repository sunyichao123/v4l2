#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <fcntl.h>

int main()
{
	//open video device
	int fd = open("/dev/video0", O_RDWR);
	if(fd < 0)
	{
		perror("open fail");
		return -1;
	}
	printf("open success fd:%d\n", fd);
	//get device info
	struct v4l2_capability cap;
	ioctl(fd, VIDIOC_QUERYCAP, &cap);
	printf("drivername:%s\ncard name:%s\nbusinfo:%s\nversion:%d\ncapabilities:%08x\n", cap.driver, cap.card, cap.bus_info,cap.version,cap.capabilities);
	if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
		printf("device is a video capture\n");

	//get frame format
	struct v4l2_fmtdesc fmt;
	fmt.index = 0;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("support format:\n");
	while(ioctl(fd, VIDIOC_ENUM_FMT, &fmt) != -1)
	{
		printf("\t%d.%s\n", fmt.index, fmt.description);
		fmt.index++;
	}

	//get current data format
	struct v4l2_format datafmt;
	datafmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(fd, VIDIOC_G_FMT, &datafmt);

	printf("current data format information:\n\twidth:%d\n\theight:%d\n", datafmt.fmt.pix.width, datafmt.fmt.pix.height);

	//申请拥有4个缓冲帧的内核空间 VIDIOC_REQBUFS
	struct v4l2_requestbuffers req;
	req.count = 4;
	req.type = V4L2_CAP_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ioctl(fd, VIDIOC_REQBUFS, &req);
	//查询内核空间地址，映射 VIDIOC_QUERYBUF
	struct v4l2_buffer mapbuffer;
	mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for(int i = 0; i < 4; i++)
	{
		mapbuffer.index = i;
		int ret = ioctl(fd, VIDIOC_QUERYBUF, &mapbuffer);
		if(ret != 0)
			perror("查询内核空间失败\n");

		//映射 mmap
		mmap();
	}
		
	
	//close the video device
	close(fd);
	return 0;
}

