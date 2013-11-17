
#ifdef BLAHBLAHBLAH
void UI_Init()
{

}

void UI_Render()
{

}

void UI_Shutdown()
{

}

#else

#include <MyGUI.h>
#include <MyGUI_OpenGLPlatform.h>


#include "gamedefs.h"
#include "r_tex.h";
#include "ui_public.h"


class UIManager : public MyGUI::OpenGLImageLoader

{
public:
    UIManager();
    virtual ~UIManager();

    static void Initialize();

    static UIManager* Instance()
    {
        return sInstance;
    }

    void Render()
    {
        mPlatform->getRenderManagerPtr()->drawOneFrame();
    }

    void Shutdown();

private:

    void _Initialize();

    void SetupResources();
    void AddResourceLocation(const std::string& _name, bool _recursive = false);
    const std::string& GetRootMedia();

    void* loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename);
    void saveImage(int _width, int _height, MyGUI::PixelFormat _format, void* _texture, const std::string& _filename);

    MyGUI::Gui* mGUI;
    MyGUI::OpenGLPlatform* mPlatform;
    std::string mRootMedia;

    static UIManager* sInstance;

};


UIManager* UIManager::sInstance = NULL;

UIManager::UIManager() :
    mGUI(NULL),
    mPlatform(NULL)
{

}

UIManager::~UIManager()
{

}

const std::string& UIManager::GetRootMedia()
{
    return mRootMedia;
}


static void rgbaToBgra(uint8_t *_data, uint32_t _width, uint32_t _height)
{
    uint32_t dstpitch = _width * 4;

    for (uint32_t yy = 0; yy < _height; ++yy)
    {
        uint8_t *dst = &_data[yy * dstpitch];

        for (uint32_t xx = 0; xx < _width; ++xx)
        {
            uint8_t tmp = dst[0];
            dst[0] = dst[2];
            dst[2] = tmp;
            dst   += 4;
        }
    }
}


void* UIManager::loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename)
{
    std::string fullname = MyGUI::OpenGLDataManager::getInstance().getDataPath(_filename);

    printf("Attempting to load image: %s\n", fullname.c_str());

    VStream* stream = FL_OpenSysFileRead(VStr(fullname.c_str()));

    if (!stream)
        Sys_Error("Failed to open stream for %s", fullname.c_str());

    VPngTexture* vtex = (VPngTexture*) VPngTexture::Create(*stream, 0);

    if (!vtex)
        Sys_Error("Failed to read image file %s", fullname.c_str());

    _width = vtex->Width;
    _height = vtex->Height;
    _format = MyGUI::PixelFormat::R8G8B8A8;

    printf("Loaded Image: %s %i %i\n", fullname.c_str(), _width, _height);

    vuint8* vpixels = vtex->GetPixels(stream);

    vuint8* pixels = (vuint8*) malloc(_width * _height * 4);
    memcpy(pixels, vpixels, _width * _height * 4);

    rgbaToBgra(pixels, _width, _height);

    delete vtex;

    stream->Close();

    delete stream;

    // leaking pixels, vtex
    return pixels;
}

void UIManager::saveImage(int _width, int _height, MyGUI::PixelFormat _format, void* _texture, const std::string& _filename)
{

}

void UIManager::AddResourceLocation(const std::string& _name, bool _recursive)
{
    mPlatform->getDataManagerPtr()->addResourceLocation(_name, _recursive);
}


void UIManager::SetupResources()
{
    MyGUI::xml::Document doc;

    if (!doc.open(std::string("resources.xml")))
        doc.getLastError();

    MyGUI::xml::ElementPtr root = doc.getRoot();
    if (root == nullptr || root->getName() != "Paths")
        return;

    MyGUI::xml::ElementEnumerator node = root->getElementEnumerator();
    while (node.next())
    {
        if (node->getName() == "Path")
        {
            if (node->findAttribute("root") != "")
            {
                bool root = MyGUI::utility::parseBool(node->findAttribute("root"));
                if (root)
                    mRootMedia = node->getContent();
            }
            AddResourceLocation(node->getContent(), false);
        }
    }

    AddResourceLocation(GetRootMedia() + "/Common/Base");
}


void UIManager::_Initialize()
{
    mPlatform = new MyGUI::OpenGLPlatform();
    mPlatform->initialise(this);

    SetupResources();

    AddResourceLocation(GetRootMedia() + "/Demos/Demo_Themes");
    //AddResourceLocation(GetRootMedia() + "/Common/Demos");
    AddResourceLocation(GetRootMedia() + "/Common/Themes");

    mGUI = new MyGUI::Gui();
    mGUI->initialise("MyGUI_Core.xml");

    // MOVE ME

    mPlatform->getRenderManagerPtr()->setViewSize(1280, 720);

    //MyGUI::ResourceManager::getInstance().load("MyGUI_BlackBlueTheme.xml");

    //MyGUI::LayoutManager::getInstance().loadLayout("Wallpaper.layout");

    //const MyGUI::VectorWidgetPtr& root = MyGUI::LayoutManager::getInstance().loadLayout("HelpPanel.layout");
    //if (root.size() == 1)
    //     root.at(0)->findWidget("Text")->castType<MyGUI::TextBox>()->setCaption("Select skin theme in combobox to see default MyGUI themes.");

    MyGUI::LayoutManager::getInstance().loadLayout("Themes.layout");


}

void UIManager::Shutdown()
{
    if (mGUI)
        mGUI->shutdown();

    if (mPlatform)
        mPlatform->shutdown();

    mGUI = NULL;
    mPlatform = NULL;
}


void UIManager::Initialize()
{
    sInstance = new UIManager();
    sInstance->_Initialize();
}



void UI_Render()
{
    if (UIManager::Instance())
        UIManager::Instance()->Render();
}

void UI_Shutdown()
{
    if (UIManager::Instance())
        UIManager::Instance()->Shutdown();
}

void UI_Init()
{
    UIManager::Initialize();
}
#endif






