/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 31.08.2026
 */

#ifndef DRIVERS_MOTOR_H_
#define DRIVERS_MOTOR_H_

#include <stdint.h>
#include <sys/cdefs.h>

#include <lib/libds/array_spread.h>

struct motor_dev;
struct pwm_device;

struct motor_msg {
	void     *mm_buf;
	int       mm_len;
	uint32_t  mm_flags;
};

#define MOTOR_TYPE_DSHOT1200    (4)
#define MOTOR_TYPE_DSHOT600     (3)
#define MOTOR_TYPE_DSHOT300     (2)
#define MOTOR_TYPE_DSHOT150     (1)

struct motor_conf {
	int mc_type;
};


struct motor_ops {
	int (*mo_init)(struct motor_dev *dev);
	int (*mo_conf)(struct motor_dev *dev, struct motor_conf *conf);
	int (*mo_send_msg)(struct motor_dev *dev, struct motor_msg *msg);
};

struct motor_dev {
	const struct motor_ops *md_ops;
	void                   *md_priv;
	int                     md_id;
	struct motor_conf       md_conf;

	struct pwm_device      *md_pwm_dev;
	int                     md_pwm_id;
	int                     md_pwm_chan;
	int                     md_period;
};

__BEGIN_DECLS
extern struct motor_dev *motor_dev_by_id(int id);

extern int motor_conf(struct motor_dev *dev, struct motor_conf *conf);
extern int motor_send_msg(struct motor_dev *dev, struct motor_msg *msg);
__END_DECLS

#define MOTOR_DEV_DEF(id, ops, priv, pwm_id, pwm_chan) \
	ARRAY_SPREAD_DECLARE(const struct motor_dev *, __motor_device_registry); \
	static struct motor_dev MACRO_CONCAT(motor_dev_, id) = \
						{ .md_id = id, \
							.md_ops = ops, \
							.md_priv = priv, \
							.md_pwm_id = pwm_id, \
							.md_pwm_chan = pwm_chan, \
						}; \
	ARRAY_SPREAD_ADD(__motor_device_registry, &MACRO_CONCAT(motor_dev_, id) )

#endif /* DRIVERS_MOTOR_H_ */
