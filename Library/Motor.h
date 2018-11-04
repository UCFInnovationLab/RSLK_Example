#ifndef MOTOR_H_
#define MOTOR_H_



typedef enum
{
    INITIAL,
    CONTINUOUS
} motor_mode_t;

void motor_init(void);
void set_left_motor_pwm(int);
void set_right_motor_pwm(int);
bool rotate_motors_by_counts(motor_mode_t, float, int, int);


#endif /* MOTOR_H_ */
