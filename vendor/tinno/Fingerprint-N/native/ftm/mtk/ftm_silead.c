/*
 **
 ** Copyright 2008, The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>  
#include <sys/mount.h>
#include <sys/statfs.h>
#include <dirent.h>
#include <linux/input.h>
#include <linux/netlink.h>  
#include <sys/socket.h>  
#include <math.h>
#include <dlfcn.h>
#include <semaphore.h>
#include <signal.h>
#include <utils/Log.h>

/************************************************************************************/
#undef LOG_TAG
#define LOG_TAG "[Tinnofingerprint-silead] "
#define TAG LOG_TAG

#define DRV_SILEAD 	 "/dev/silead_fp_dev"

#define  SPI_IOC_MAGIC    'k' 

#define  IOCTL_FINGER_STATE_INFO	_IOR(SPI_IOC_MAGIC, 81, __u32)

/************************************************************************************/

static int _dev_fd = -1;
static int fp_exit;
static sem_t wk_sem;
/************************************************************************************/
int silead_dev_init(void);
int silead_dev_uninit(void);
int silead_update_finger_state(int *s);

/************************************************************************************/
static int finger_state = 0;

static void fasync_input_handler(int signum)  
{
	ALOGE(TAG "[%s] : %d \n", __func__, signum);

	int ret = 0;
        int state = 0;
        if((ret = ioctl(_dev_fd, IOCTL_FINGER_STATE_INFO, &state)) < 0)
	{
                state = 0;
		ALOGE(TAG "--ioctl--  err!  ret  = [%d]\n",ret);  
	}
        finger_state = state;		 
}

static void register_fasync_notify(int fd)
{
	int flag; 

	ALOGE(TAG"[%s]\n", __func__);
	signal(SIGIO, fasync_input_handler);
	flag = fcntl(fd, F_GETFL);
	flag |= FASYNC;
	fcntl(fd, F_SETFL, flag);
	fcntl(fd, F_SETOWN, getpid());
} 
  
int silead_dev_init(void)
{
	int fd;

	ALOGE(TAG "-->[%s]\n",__func__);  

	if((fd = open(DRV_SILEAD, O_RDWR)) < 0)
	{
		ALOGE(TAG "open:%s err!\n", DRV_SILEAD);  
		return -1;
	}

	_dev_fd = fd;  
	fp_exit = 0;
	
	register_fasync_notify(fd);
	return 0;
}

int silead_dev_uninit(void)
{
	int fd = _dev_fd;
	ALOGE(TAG "-->[%s]\n",__func__);  
	
	if(fd > 0)
	{
		close(fd);
		_dev_fd = -1;
		fp_exit = 1;
                finger_state = -1;
		return 0;
	}
	
	return -1;
}

int silead_update_finger_state(int *s) 
{
	int state = 0;
	int ret = 0;

	if(fp_exit) 
	{ 
		ALOGE(TAG "[%s]: Exit~\n",__func__);  
		return -1; 
	}
	
	*s = finger_state == 1 ? 1 : 0;

	ALOGE(TAG "state = [%d]\n",finger_state);  

	return 0;
}

