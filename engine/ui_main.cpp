
#include "ui_public.h"

#include <MyGUI.h>
#include <MyGUI_OpenGLPlatform.h>

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


void* UIManager::loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename)
{
    return NULL;
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

    mGUI = new MyGUI::Gui();
    mGUI->initialise("MyGUI_Core.xml");

}

void UIManager::Initialize()
{
    sInstance = new UIManager();
    sInstance->_Initialize();
}


void UI_Init()
{
    UIManager::Initialize();
}







