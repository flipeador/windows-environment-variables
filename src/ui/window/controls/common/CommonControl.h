struct CommonControlFont
{
    HFONT hFont;
    INT size;
};

class CommonControl abstract : public Window
{
public:
    explicit CommonControl(WSTRV className, HWND hWndParent, UINT style, UINT exStyle);
    virtual ~CommonControl();

    virtual HFONT SetFont(WSTRV name, INT size = 16, INT weight = 0);

    static CommonControl* GetInstance(HWND hWnd);
protected:
    virtual VOID OnDpiChanged() override;
private:
    CommonControlFont m_font { };
};
