/*
 * Copyright (c) 2000-2021 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
#ifndef FSEVENT_H
#define FSEVENT_H 1

#include <stdint.h>
#include <sys/types.h>

// Event types that you can ask to listen for
#define FSE_INVALID             -1
#define FSE_CREATE_FILE          0
#define FSE_DELETE               1
#define FSE_STAT_CHANGED         2
#define FSE_RENAME               3
#define FSE_CONTENT_MODIFIED     4
#define FSE_EXCHANGE             5
#define FSE_FINDER_INFO_CHANGED  6
#define FSE_CREATE_DIR           7
#define FSE_CHOWN                8
#define FSE_XATTR_MODIFIED       9
#define FSE_XATTR_REMOVED       10
#define FSE_DOCID_CREATED       11
#define FSE_DOCID_CHANGED       12
#define FSE_UNMOUNT_PENDING     13 // iOS-only: client must respond via FSEVENTS_UNMOUNT_PENDING_ACK
#define FSE_CLONE               14

#define FSE_ACTIVITY            15
#define FSE_ACTIVITY_VERSION_1  1

#define FSE_ACTIVITY_ATTR_DEV            0x00000001
#define FSE_ACTIVITY_ATTR_INO            0x00000002
#define FSE_ACTIVITY_ATTR_ORIGIN_ID      0x00000004
#define FSE_ACTIVITY_ATTR_AGE            0x00000008
#define FSE_ACTIVITY_ATTR_USE_STATE      0x00000010
#define FSE_ACTIVITY_ATTR_URGENCY        0x00000020
#define FSE_ACTIVITY_ATTR_SIZE           0x00000040

#define FSE_MAX_EVENTS          16
#define FSE_ACCESS_GRANTED      FSE_INVALID
#ifdef BSD_KERNEL_PRIVATE
#define FSE_CONTENT_MODIFIED_NO_HLINK  997
#endif /* BSD_KERNEL_PRIVATE */
#define FSE_ALL_EVENTS         998

#define FSE_EVENTS_DROPPED     999

//
// These defines only apply if you've asked for extended
// type info.  In that case the type field's low 12-bits
// contain the basic types defined above and the top 4
// bits contain flags that indicate if an event had other
// events combined with it or if it represents a directory
// that has dropped events below it.
//
#define FSE_TYPE_MASK          0x0fff
#define FSE_FLAG_MASK          0xf000
#define FSE_FLAG_SHIFT         12
#define FSE_GET_FLAGS(type)    (((type) >> 12) & 0x000f)

#define FSE_COMBINED_EVENTS          0x0001
#define FSE_CONTAINS_DROPPED_EVENTS  0x0002


// Actions for each event type
#define FSE_IGNORE    0
#define FSE_REPORT    1
#define FSE_ASK       2    // Not implemented yet

// The default disposition is to not report FSE_ACCESS_GRANTED
// events.  (They are not useful to the vast majority of
// /dev/fsevents clients.)
#define FSE_REPORT_DEFAULT(e) \
    ((e) == FSE_ACCESS_GRANTED ? FSE_IGNORE : FSE_REPORT)

// The types of each of the arguments for an event
// Each type is followed by the size and then the
// data.  FSE_ARG_VNODE is just a path string
// FSE_ARG_AUDIT_TOKEN is only delivered on FSE_ACCESS_GRANTED
// events.
#define FSE_ARG_VNODE    0x0001   // next arg is a vnode pointer
#define FSE_ARG_STRING   0x0002   // next arg is length followed by string ptr
#define FSE_ARG_PATH     0x0003   // next arg is a full path
#define FSE_ARG_INT32    0x0004   // next arg is a 32-bit int
#define FSE_ARG_INT64    0x0005   // next arg is a 64-bit int
#define FSE_ARG_RAW      0x0006   // next arg is a length followed by a void ptr
#define FSE_ARG_INO      0x0007   // next arg is the inode number (ino_t)
#define FSE_ARG_UID      0x0008   // next arg is the file's uid (uid_t)
#define FSE_ARG_DEV      0x0009   // next arg is the file's dev_t
#define FSE_ARG_MODE     0x000a   // next arg is the file's mode (as an int32, file type only)
#define FSE_ARG_GID      0x000b   // next arg is the file's gid (gid_t)
#define FSE_ARG_FINFO    0x000c   // next arg is a packed finfo (dev, ino, mode, uid, gid)
#define FSE_ARG_AUDIT_TOKEN 0x000d // next arg is an audit_token_t ptr
#define FSE_ARG_DONE     0xb33f   // no more arguments

#define FSE_MAX_ARGS     13

//
// These are special bits that be set in the 32-bit mode
// field that /dev/fsevents provides.
//
#define FSE_MODE_HLINK         (1U << 31)    // notification is for a hard-link
#define FSE_MODE_LAST_HLINK    (1U << 30)    // link count == 0 on a hard-link delete
#define FSE_REMOTE_DIR_EVENT   (1U << 29)    // this is a remotely generated directory-level granularity event
#define FSE_TRUNCATED_PATH     (1U << 28)    // the path for this item had to be truncated
#define FSE_MODE_CLONE         (1U << 27)    // notification is for a clone

// ioctl's on /dev/fsevents
typedef struct fsevent_clone_args {
	int8_t  *event_list;
	int32_t  num_events;
	int32_t  event_queue_depth;
	int32_t *fd;
} fsevent_clone_args;

#define FSEVENTS_CLONE          _IOW('s', 1, fsevent_clone_args)


// ioctl's on the cloned fd
#pragma pack(push, 4)
typedef struct fsevent_dev_filter_args {
	uint32_t  num_devices;
	dev_t    *devices;
} fsevent_dev_filter_args;
#pragma pack(pop)

#define FSEVENTS_DEVICE_FILTER          _IOW('s', 100, fsevent_dev_filter_args)
#define FSEVENTS_WANT_COMPACT_EVENTS    _IO('s', 101)
#define FSEVENTS_WANT_EXTENDED_INFO     _IO('s', 102)
#define FSEVENTS_GET_CURRENT_ID         _IOR('s', 103, uint64_t)
#define FSEVENTS_UNMOUNT_PENDING_ACK    _IOW('s', 104, dev_t)


#ifdef BSD_KERNEL_PRIVATE

void fsevents_init(void);
void fsevent_unmount(struct mount *mp, vfs_context_t ctx);
struct vnode_attr;
void create_fsevent_from_kevent(vnode_t vp, uint32_t kevents, struct vnode_attr *vap);

// misc utility functions for fsevent info and pathbuffers...
typedef struct fse_info {
	ino64_t    ino;
	dev_t      dev;
	int32_t    mode;// note: this is not a mode_t (it's 32-bits, not 16)
	uid_t      uid;
	uint32_t   document_id;
	uint64_t   nlink;// only filled in if the vnode is marked as a hardlink
} fse_info;

int   get_fse_info(struct vnode *vp, fse_info *fse, vfs_context_t ctx);
int   vnode_get_fse_info_from_vap(vnode_t vp, fse_info *fse, struct vnode_attr *vap);

char *get_pathbuff(void);
void  release_pathbuff(char *path);

#endif /* BSD_KERNEL_PRIVATE */

#ifdef KERNEL_PRIVATE

int  need_fsevent(int type, vnode_t vp);
int  add_fsevent(int type, vfs_context_t, ...);
int  test_fse_access_granted(vnode_t vp, unsigned long type, vfs_context_t);

#endif /* KERNEL_PRIVATE */

#endif /* FSEVENT_H */
