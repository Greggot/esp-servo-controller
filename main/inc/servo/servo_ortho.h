/**
 * @file
 * @brief   Servo methods
 * @author https://github.com/Greggot/
 */
#pragma once 

namespace servo {

void omega_init(void);
void theta_init(void);

void set_omega_angle(unsigned int grad);
void set_theta_angle(unsigned int grad);

}