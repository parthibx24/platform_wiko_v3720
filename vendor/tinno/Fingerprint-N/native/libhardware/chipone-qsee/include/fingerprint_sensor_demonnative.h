#ifndef FINGERPRINT_SENSOR_DAEMON_H
#define FINGERPRINT_SENSOR_DAEMON_H

#include "fingerprint_sensor.h"

#ifdef NDK_ROOT
#include "hw_auth_token.h"
#else
#include "hardware/hw_auth_token.h"
#endif

// #if (TARGET_ANDROID >= 7)
// #error: android 7
// #elif (TARGET_ANDROID == 6)
// #error: android 6
// #else
// #error: android 5
// #endif

#define FPSENSOR_FINGERPRINT_HARDWARE_MODULE_ID  "fingerprint.chipone" //fpd hal
#define FINGERPRINT_MODULE_API_VERSION_1_0 HARDWARE_MODULE_API_VERSION(1, 0)  //for fpd hal 
#define FINGERPRINT_MODULE_API_VERSION_2_0 HARDWARE_MODULE_API_VERSION(2, 0)  //for fpd hal

typedef enum fingerprint_msg_type
{
    FINGERPRINT_ERROR = -1,
    FINGERPRINT_ACQUIRED = 1,
    FINGERPRINT_TEMPLATE_ENROLLING = 3,
    FINGERPRINT_TEMPLATE_REMOVED = 4,
    FINGERPRINT_AUTHENTICATED = 5,
#if (TARGET_ANDROID>=7)
    FINGERPRINT_TEMPLATE_ENUMERATING = 6,
#endif    
} fingerprint_msg_type_t;

/*
 * Fingerprint errors are meant to tell the framework to terminate the current operation and ask
 * for the user to correct the situation. These will almost always result in messaging and user
 * interaction to correct the problem.
 *
 * For example, FINGERPRINT_ERROR_CANCELED should follow any acquisition message that results in
 * a situation where the current operation can't continue without user interaction. For example,
 * if the sensor is dirty during enrollment and no further enrollment progress can be made,
 * send FINGERPRINT_ACQUIRED_IMAGER_DIRTY followed by FINGERPRINT_ERROR_CANCELED.
 */
typedef enum fingerprint_error
{
    FINGERPRINT_ERROR_HW_UNAVAILABLE = 1, /* The hardware has an error that can't be resolved. */
    FINGERPRINT_ERROR_UNABLE_TO_PROCESS = 2, /* Bad data; operation can't continue */
    FINGERPRINT_ERROR_TIMEOUT = 3, /* The operation has timed out waiting for user input. */
    FINGERPRINT_ERROR_NO_SPACE = 4, /* No space available to store a template */
    FINGERPRINT_ERROR_CANCELED = 5, /* The current operation can't proceed. See above. */
    FINGERPRINT_ERROR_UNABLE_TO_REMOVE = 6, /* fingerprint with given id can't be removed */
    FINGERPRINT_ERROR_VENDOR_BASE = 1000 /* vendor-specific error messages start here */
} fingerprint_error_t;

/*
 * Fingerprint acquisition info is meant as feedback for the current operation.  Anything but
 * FINGERPRINT_ACQUIRED_GOOD will be shown to the user as feedback on how to take action on the
 * current operation. For example, FINGERPRINT_ACQUIRED_IMAGER_DIRTY can be used to tell the user
 * to clean the sensor.  If this will cause the current operation to fail, an additional
 * FINGERPRINT_ERROR_CANCELED can be sent to stop the operation in progress (e.g. enrollment).
 * In general, these messages will result in a "Try again" message.
 */
typedef enum fingerprint_acquired_info
{
    FINGERPRINT_ACQUIRED_GOOD = 0,
    FINGERPRINT_ACQUIRED_PARTIAL = 1, /* sensor needs more data, i.e. longer swipe. */
    FINGERPRINT_ACQUIRED_INSUFFICIENT = 2, /* image doesn't contain enough detail for recognition*/
    FINGERPRINT_ACQUIRED_IMAGER_DIRTY = 3, /* sensor needs to be cleaned */
    FINGERPRINT_ACQUIRED_TOO_SLOW = 4, /* mostly swipe-type sensors; not enough data collected */
    FINGERPRINT_ACQUIRED_TOO_FAST = 5, /* for swipe and area sensors; tell user to slow down*/
    FINGERPRINT_ACQUIRED_VENDOR_BASE = 1000, /* vendor-specific acquisition messages start here */


//   FINGERPRINT_ACQUIRED_VENDOR_CAPTURE_FAST = FINGERPRINT_ACQUIRED_TOO_FAST,


//    FINGERPRINT_ACQUIRED_VENDOR_CAPTURE_EIO = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 0,
//    FINGERPRINT_ACQUIRED_VENDOR_CAPTURE_EAGAIN = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 1,
    FINGERPRINT_ACQUIRED_VENDOR_CAPTURE_MOVE = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 2,
//    FINGERPRINT_ACQUIRED_VENDOR_CAPTURE_TIMEOUT = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 3,
    FINGERPRINT_ACQUIRED_VENDOR_CAPTURE_FINGERDOWN = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 4,
    FINGERPRINT_ACQUIRED_VENDOR_CAPTURE_FINGERUP = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 5,
    FINGERPRINT_ACQUIRED_VENDOR_CAPTURE_UNKNOWN = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 6,

    FINGERPRINT_ACQUIRED_VENDOR_ALGO_4103 = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 7,  //Move finger more
    FINGERPRINT_ACQUIRED_VENDOR_ALGO_4104 = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 8,  //Do not move finger too much
    FINGERPRINT_ACQUIRED_VENDOR_ALGO_IMAGEENROLLED = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 9,
    FINGERPRINT_ACQUIRED_VENDOR_ALGO_FILL_IND_UP = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 10,
    FINGERPRINT_ACQUIRED_VENDOR_ALGO_FILL_IND_DOWN = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 11,
    FINGERPRINT_ACQUIRED_VENDOR_ALGO_FILL_IND_LEFT = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 12,
    FINGERPRINT_ACQUIRED_VENDOR_ALGO_FILL_IND_RIGHT = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 13,
    FINGERPRINT_ACQUIRED_VENDOR_ALGO_FILL_IND_UNKNOWN = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 14,
    FINGERPRINT_ACQUIRED_VENDOR_ALGO_UNKNOWN = FINGERPRINT_ACQUIRED_VENDOR_BASE  + 15,
} fingerprint_acquired_info_t;

#define FP_SERVICE_CONTROL_CMD_BASE                         0
#define FP_SERVICE_CONTROL_CMD_SETENROLL_IMGCNT             (FP_SERVICE_CONTROL_CMD_BASE + 1)
#define FP_SERVICE_CONTROL_CMD_CAPTURE_CANCEL               (FP_SERVICE_CONTROL_CMD_BASE + 2)

//for sensor navigator control, save status
#define FP_SERVICE_CONTROL_CMD_NAVIGATOR_ENABLE             (FP_SERVICE_CONTROL_CMD_BASE + 100)
#define FP_SERVICE_CONTROL_CMD_NAVIGATOR_EVENT_REPORT       (FP_SERVICE_CONTROL_CMD_BASE + 101)
#define FP_SERVICE_CONTROL_CMD_NAVIGATOR_ENABLE_REPORT      (FP_SERVICE_CONTROL_CMD_BASE + 102)
#define FP_SERVICE_CONTROL_CMD_NAVIGATOR_DISABLE_REPORT     (FP_SERVICE_CONTROL_CMD_BASE + 103)
#define FP_SERVICE_CONTROL_CMD_NAVIGATOR_GET_STATUS         (FP_SERVICE_CONTROL_CMD_BASE + 104)

#define FP_SERVICE_CONTROL_CMD_CHECK_BOARD                  (FP_SERVICE_CONTROL_CMD_BASE + 110)
#define FP_SERVICE_CONTROL_CMD_SELF_TEST                    (FP_SERVICE_CONTROL_CMD_BASE + 111)
#define FP_SERVICE_CONTROL_CMD_FINGER_DETECT_TEST           (FP_SERVICE_CONTROL_CMD_BASE + 112)

//for rongqi project internal use
//control to pause and resume enroll
#define FP_SERVICE_CONTROL_CMD_ENROLL_PAUSE                 (FP_SERVICE_CONTROL_CMD_BASE + 5000)
#define FP_SERVICE_CONTROL_CMD_ENROLL_RESUME                (FP_SERVICE_CONTROL_CMD_BASE + 5001)
//end for rongqi

//start sensor test activity cmd, for shell "service call" cmd
#define FP_SERVICE_CONTROL_CMD_MISC_START_TEST_ACTIVITY     (FP_SERVICE_CONTROL_CMD_BASE + 5010)
#define FP_SERVICE_CONTROL_CMD_MISC_PRINT_SYSTEM_INFO       (FP_SERVICE_CONTROL_CMD_BASE + 5011)
//enable logd print even in release mode value = 1 enable; = 0 disable; default is 0
#define FP_SERVICE_CONTROL_CMD_RELEASE_MODE_ENABLE_LOGD     (FP_SERVICE_CONTROL_CMD_BASE + 5012)
#define FP_SERVICE_CONTROL_CMD_INJECTION_CONTROL            (FP_SERVICE_CONTROL_CMD_BASE + 5013)
#define FP_SERVICE_CONTROL_CMD_INJECTION_IMG_IDX            (FP_SERVICE_CONTROL_CMD_BASE + 5014)
#define FP_SERVICE_CONTROL_CMD_INJECTION_AUTHENTICATE_TEST  (FP_SERVICE_CONTROL_CMD_BASE + 5015)
#define FP_SERVICE_CONTROL_CMD_REBOOT_PHONE                 (FP_SERVICE_CONTROL_CMD_BASE + 5016)
#define FP_SERVICE_CONTROL_CMD_EMULATE_POWER_KEY            (FP_SERVICE_CONTROL_CMD_BASE + 5017)
#define FP_SERVICE_CONTROL_CMD_MISC_CONTROL_AGING_TEST      (FP_SERVICE_CONTROL_CMD_BASE + 5018)
#define FP_SERVICE_CONTROL_CMD_MISC_CONTROL_REBOOT_AUTH_TEST (FP_SERVICE_CONTROL_CMD_BASE + 5019)

typedef struct fingerprint_finger_id
{
    uint32_t gid;
    uint32_t fid;
} fingerprint_finger_id_t;

typedef struct fingerprint_enroll
{
    fingerprint_finger_id_t finger;
    /* samples_remaining goes from N (no data collected, but N scans needed)
     * to 0 (no more data is needed to build a template). */
    uint32_t samples_remaining;
    uint64_t msg; /* Vendor specific message. Used for user guidance */
} fingerprint_enroll_t;

#if (TARGET_ANDROID>=7)
typedef struct fingerprint_enumerated 
{
    fingerprint_finger_id_t finger;
    uint32_t remaining_templates;
} fingerprint_enumerated_t;
#endif

typedef struct fingerprint_removed
{
    fingerprint_finger_id_t finger;
} fingerprint_removed_t;

typedef struct fingerprint_acquired
{
    fingerprint_acquired_info_t acquired_info; /* information about the image */
} fingerprint_acquired_t;

typedef struct fingerprint_authenticated
{
    fingerprint_finger_id_t finger;
    hw_auth_token_t hat;
} fingerprint_authenticated_t;

typedef struct fingerprint_msg
{
    fingerprint_msg_type_t type;
    union
    {
        fingerprint_error_t error;
        fingerprint_enroll_t enroll;
#if (TARGET_ANDROID>=7)
        fingerprint_enumerated_t enumerated;
#endif
        fingerprint_removed_t removed;
        fingerprint_acquired_t acquired;
        fingerprint_authenticated_t authenticated;
    } data;
} fingerprint_msg_t;

/* Callback function type */
typedef void (*fingerprint_notify_t)(const fingerprint_msg_t *msg, void *env);

typedef struct fingerprint_device
{
    /**
     * Common methods of the fingerprint device. This *must* be the first member
     * of fingerprint_device as users of this structure will cast a hw_device_t
     * to fingerprint_device pointer in contexts where it's known
     * the hw_device_t references a fingerprint_device.
     */
    struct hw_device_t common;

    /*
     * Client provided callback function to receive notifications.
     * Do not set by hand, use the function above instead.
     */
    fingerprint_notify_t notify;

    /*
     * Set notification callback:
     * Registers a user function that would receive notifications from the HAL
     * The call will block if the HAL state machine is in busy state until HAL
     * leaves the busy state.
     *
     * Function return: 0 if callback function is successfuly registered
     *                  or a negative number in case of error, generally from the errno.h set.
     */
    int (*set_notify)(struct fingerprint_device *dev, fingerprint_notify_t notify);

    /*
     * Fingerprint pre-enroll enroll request:
     * Generates a unique token to upper layers to indicate the start of an enrollment transaction.
     * This token will be wrapped by security for verification and passed to enroll() for
     * verification before enrollment will be allowed. This is to ensure adding a new fingerprint
     * template was preceded by some kind of credential confirmation (e.g. device password).
     *
     * Function return: 0 if function failed
     *                  otherwise, a uint64_t of token
     */
    uint64_t (*pre_enroll)(struct fingerprint_device *dev);

    /*
     * Fingerprint enroll request:
     * Switches the HAL state machine to collect and store a new fingerprint
     * template. Switches back as soon as enroll is complete
     * (fingerprint_msg.type == FINGERPRINT_TEMPLATE_ENROLLING &&
     *  fingerprint_msg.data.enroll.samples_remaining == 0)
     * or after timeout_sec seconds.
     * The fingerprint template will be assigned to the group gid. User has a choice
     * to supply the gid or set it to 0 in which case a unique group id will be generated.
     *
     * Function return: 0 if enrollment process can be successfully started
     *                  or a negative number in case of error, generally from the errno.h set.
     *                  A notify() function may be called indicating the error condition.
     */
    int (*enroll)(struct fingerprint_device *dev, const hw_auth_token_t *hat,
                  uint32_t gid, uint32_t timeout_sec);

    /*
     * Finishes the enroll operation and invalidates the pre_enroll() generated challenge.
     * This will be called at the end of a multi-finger enrollment session to indicate
     * that no more fingers will be added.
     *
     * Function return: 0 if the request is accepted
     *                  or a negative number in case of error, generally from the errno.h set.
     */
    int (*post_enroll)(struct fingerprint_device *dev);

    /*
     * get_authenticator_id:
     * Returns a token associated with the current fingerprint set. This value will
     * change whenever a new fingerprint is enrolled, thus creating a new fingerprint
     * set.
     *
     * Function return: current authenticator id or 0 if function failed.
     */
    uint64_t (*get_authenticator_id)(struct fingerprint_device *dev);

    /*
     * Cancel pending enroll or authenticate, sending FINGERPRINT_ERROR_CANCELED
     * to all running clients. Switches the HAL state machine back to the idle state.
     * Unlike enroll_done() doesn't invalidate the pre_enroll() challenge.
     *
     * Function return: 0 if cancel request is accepted
     *                  or a negative number in case of error, generally from the errno.h set.
     */
    int (*cancel)(struct fingerprint_device *dev);

    /*
     * Enumerate all the fingerprint templates found in the directory set by
     * set_active_group()
     * This is a synchronous call. The function takes:
     * - A pointer to an array of fingerprint_finger_id_t.
     * - The size of the array provided, in fingerprint_finger_id_t elements.
     * Max_size is a bi-directional parameter and returns the actual number
     * of elements copied to the caller supplied array.
     * In the absence of errors the function returns the total number of templates
     * in the user directory.
     * If the caller has no good guess on the size of the array he should call this
     * function witn *max_size == 0 and use the return value for the array allocation.
     * The caller of this function has a complete list of the templates when *max_size
     * is the same as the function return.
     *
     * Function return: Total number of fingerprint templates in the current storage directory.
     *                  or a negative number in case of error, generally from the errno.h set.
     */
#if (TARGET_ANDROID >= 7)
    int (*enumerate)(struct fingerprint_device *dev);
#else
    int (*enumerate)(struct fingerprint_device *dev, fingerprint_finger_id_t *results,
                     uint32_t *max_size);
#endif

    /*
     * Fingerprint remove request:
     * Deletes a fingerprint template.
     * Works only within a path set by set_active_group().
     * notify() will be called with details on the template deleted.
     * fingerprint_msg.type == FINGERPRINT_TEMPLATE_REMOVED and
     * fingerprint_msg.data.removed.id indicating the template id removed.
     *
     * Function return: 0 if fingerprint template(s) can be successfully deleted
     *                  or a negative number in case of error, generally from the errno.h set.
     */
    int (*remove)(struct fingerprint_device *dev, uint32_t gid, uint32_t fid);

    /*
     * Restricts the HAL operation to a set of fingerprints belonging to a
     * group provided.
     * The caller must provide a path to a storage location within the user's
     * data directory.
     *
     * Function return: 0 on success
     *                  or a negative number in case of error, generally from the errno.h set.
     */
    int (*set_active_group)(struct fingerprint_device *dev, uint32_t gid,
                            const char *store_path);

    /*
     * Authenticates an operation identifed by operation_id
     *
     * Function return: 0 on success
     *                  or a negative number in case of error, generally from the errno.h set.
     */
    int (*authenticate)(struct fingerprint_device *dev, uint64_t operation_id, uint32_t gid);

    /* Reserved for backward binary compatibility */
    void *reserved[4];
} fingerprint_device_t;

typedef struct fingerprint_module
{
    /**
     * Common methods of the fingerprint module. This *must* be the first member
     * of fingerprint_module as users of this structure will cast a hw_module_t
     * to fingerprint_module pointer in contexts where it's known
     * the hw_module_t references a fingerprint_module.
     */
    struct hw_module_t common;
} fingerprint_module_t;

#endif // FINGERPRINT_SENSOR_DAEMON_H

