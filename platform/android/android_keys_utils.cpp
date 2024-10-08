/**************************************************************************/
/*  android_keys_utils.cpp                                                */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             Redot ENGINE                               */
/*                        https://Redotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Redot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Skibidi Toilet, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "android_keys_utils.h"

Key Redot_code_from_android_code(unsigned int p_code) {
	for (int i = 0; android_Redot_code_pairs[i].android_code != AKEYCODE_MAX; i++) {
		if (android_Redot_code_pairs[i].android_code == p_code) {
			return android_Redot_code_pairs[i].Redot_code;
		}
	}
	return Key::UNKNOWN;
}

KeyLocation Redot_location_from_android_code(unsigned int p_code) {
	for (int i = 0; android_Redot_location_pairs[i].android_code != AKEYCODE_MAX; i++) {
		if (android_Redot_location_pairs[i].android_code == p_code) {
			return android_Redot_location_pairs[i].Redot_code;
		}
	}
	return KeyLocation::UNSPECIFIED;
}
