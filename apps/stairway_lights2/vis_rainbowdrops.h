
/*
 * File: basicfadeingamma
 * 
 * By: Andrew Tuline
 * 
 * Date: April, 2019
 * 
 * Based previous work (namely twinklefox) by Mark Kriegsman, this program shows how you can fade-in twinkles by using the fact that a random number generator
 * with the same seed will generate the same numbers every time. Combine that with millis and a sine wave and you have twinkles fading in/out.
 * 
 * The problem is that changes above 50 are much less noticeable to the viewer, so the LED gets bright quickly and then stays bright for way too long before
 * dimming again. This version also includes gamma correction to change LED brightness so that it appears to change evenly for the viewer. 
 * 
 */

                                      // Number of LED's.

int basicfadeingamma_loop(bool clear, bool sensor_active_top, bool sensor_active_bottom);