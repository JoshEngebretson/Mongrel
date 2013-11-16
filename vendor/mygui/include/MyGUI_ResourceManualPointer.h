/*
 * This source file is part of MyGUI. For the latest info, see http://mygui.info/
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef __MYGUI_RESOURCE_MANUAL_POINTER_H__
#define __MYGUI_RESOURCE_MANUAL_POINTER_H__

#include "MyGUI_Prerequest.h"
#include "MyGUI_IPointer.h"

namespace MyGUI
{

	class MYGUI_EXPORT ResourceManualPointer :
		public IPointer
	{
		MYGUI_RTTI_DERIVED( ResourceManualPointer )

	public:
		ResourceManualPointer();
		virtual ~ResourceManualPointer();

		virtual void deserialization(xml::ElementPtr _node, Version _version);

		virtual void setImage(ImageBox* _image);
		virtual void setPosition(ImageBox* _image, const IntPoint& _point);

	private:
		IntPoint mPoint;
		IntSize mSize;
		IntCoord mTextureCoord;
		std::string mTexture;
	};

} // namespace MyGUI

#endif // __MYGUI_RESOURCE_MANUAL_POINTER_H__
