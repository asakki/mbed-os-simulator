/* mbed Microcontroller Library
 * Copyright (c) 2013 Nordic Semiconductor
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "port_api.h"
#include "pinmap.h"
#include "gpio_api.h"

PinName port_pin(PortName port, int pin_n)
{
    return (PinName)(pin_n);
}

void port_init(port_t *obj, PortName port, int mask, PinDirection dir)
{
    obj->port = port;
    obj->mask = mask;

    for (int i = 0; i<31; i++) {
        if (obj->mask & (1 << i)) {
            EM_ASM_({
                MbedJSHal.gpio.init($0, $1);
            }, obj, port_pin(obj->port, i));
        }
    }

    port_dir(obj, dir);
}

void port_mode(port_t *obj, PinMode mode)
{
    // The mode is set per pin: reuse pinmap logic
    for (int i = 0; i<31; i++) {
        if (obj->mask & (1 << i)) {
            EM_ASM_({
                MbedJSHal.gpio.mode($0, $1);
            }, port_pin(obj->port, i), mode);
        }
    }
}

void port_dir(port_t *obj, PinDirection direction)
{
    for (int i = 0; i<31; i++) {
        if (obj->mask & (1 << i)) {
            EM_ASM_({
                MbedJSHal.gpio.dir($0, $1);
            }, port_pin(obj->port, i), direction);
        }
    }
}

void port_write(port_t *obj, int value)
{
    for (int i = 0; i<31; i++) {
        if (obj->mask & (1 << i)) {
            EM_ASM_({
                MbedJSHal.gpio.write($0, $1);
            }, port_pin(obj->port, i), (1UL & (value >> i)));
        }
    }
}

int port_read(port_t *obj)
{
    int result = 0;
    for (int i = 0; i<31; i++) {
        if (obj->mask & (1 << i)) {
            int val = EM_ASM_INT({
                MbedJSHal.gpio.read($0);
            }, port_pin(obj->port, i));
            result |= val << i;
        }
    }
    return result;
}
