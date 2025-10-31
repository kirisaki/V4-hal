#ifndef V4_HAL_ERROR_H
#define V4_HAL_ERROR_H

/**
 * @file hal_error.h
 * @brief Error code definitions for V4 HAL
 *
 * All HAL functions return int (v4_err compatible).
 * 0 indicates success, negative values indicate errors.
 */

#define HAL_OK 0           /**< Success */
#define HAL_ERR_PARAM -1   /**< Invalid parameter */
#define HAL_ERR_BUSY -2    /**< Resource busy */
#define HAL_ERR_TIMEOUT -3 /**< Operation timed out */
#define HAL_ERR_NODEV -4   /**< Device not found */
#define HAL_ERR_NOMEM -5   /**< Out of memory */
#define HAL_ERR_NOTSUP -6  /**< Feature not supported */
#define HAL_ERR_IO -7      /**< I/O error */

#endif  // V4_HAL_ERROR_H
