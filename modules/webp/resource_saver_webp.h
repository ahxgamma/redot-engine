/**************************************************************************/
/*  resource_saver_webp.h                                                 */
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

#ifndef RESOURCE_SAVER_WEBP_H
#define RESOURCE_SAVER_WEBP_H

#include "core/io/image.h"
#include "core/io/resource_saver.h"

class ResourceSaverWebP : public ResourceFormatSaver {
public:
	static Error save_image(const String &p_path, const Ref<Image> &p_img, const bool p_lossy = false, const float p_quality = 0.75f);
	static Vector<uint8_t> save_image_to_buffer(const Ref<Image> &p_img, const bool p_lossy = false, const float p_quality = 0.75f);

	virtual Error save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags = 0) override;
	virtual bool recognize(const Ref<Resource> &p_resource) const override;
	virtual void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const override;

	ResourceSaverWebP();
};

#endif // RESOURCE_SAVER_WEBP_H
