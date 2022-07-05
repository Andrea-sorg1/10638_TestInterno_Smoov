object FormInput: TFormInput
  Left = 1200
  Top = 261
  AlphaBlendValue = 128
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Input'
  ClientHeight = 327
  ClientWidth = 224
  Color = clBackground
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010100001000400280100001600000028000000100000002000
    0000010004000000000000000000000000000000000000000000000000000000
    0000000080000080000000808000800000008000800080800000C0C0C0008080
    80000000FF0000FF000000FFFF00FF0000004200B300FFFF0000FFFFFF000000
    FFFFFFFF000000FFDDDDDDDDFF000FDDFFFFFFFFDDF00FDFFFFDDFFFFDF0FDFF
    FFFDDFFFFFDFFDFFFFFDDFFFFFDFFDFFFFFDDFFFFFDFFDFFFFFDDDDDFFDFFDFD
    DFFDDDDDFFDFFDFDDFFDDFFFFFDFFDFDDFFDDFFFFFDFFDFDDFFDDDDDFFDFFFFD
    DFFDDDDDFDF0FDDDDDDFFFFFDDF0FDDDDDDFDDDDFF00FFFFFFFFFFFF0000F00F
    F204C003F2048001F2048001F2040000F2040000F2040000F2040000F2040000
    F2040000F2040000F2040000F2040001F2040001F2040003F204000FF204}
  KeyPreview = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnActivate = FormActivate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 224
    Height = 327
    Align = alClient
    BevelInner = bvLowered
    TabOrder = 0
    object lblTimeStart: TLabel
      Left = 4
      Top = 80
      Width = 49
      Height = 13
      Alignment = taCenter
      Caption = 'Time Start'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Bevel3: TBevel
      Left = 13
      Top = 180
      Width = 200
      Height = 8
      Shape = bsTopLine
    end
    object lblMacchina: TLabel
      Left = 4
      Top = 20
      Width = 44
      Height = 13
      Caption = 'Macchina'
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentColor = False
      ParentFont = False
    end
    object lblTimeEnd: TLabel
      Left = 4
      Top = 135
      Width = 43
      Height = 13
      Alignment = taCenter
      Caption = 'Time End'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object lblTipo: TLabel
      Left = 127
      Top = 20
      Width = 20
      Height = 13
      Caption = 'Tipo'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Bevel2: TBevel
      Left = 13
      Top = 125
      Width = 200
      Height = 8
      Shape = bsTopLine
    end
    object Bevel1: TBevel
      Left = 13
      Top = 68
      Width = 200
      Height = 8
      Shape = bsTopLine
    end
    object Bevel4: TBevel
      Left = 13
      Top = 270
      Width = 200
      Height = 8
      Shape = bsTopLine
    end
    object lblInputString: TLabel
      Left = 13
      Top = 190
      Width = 200
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'Input String'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object txtEdit1: TEdit
      Left = 7
      Top = 5
      Width = 210
      Height = 21
      TabOrder = 0
      Visible = False
    end
    object BtnOK: TBitBtn
      Left = 13
      Top = 280
      Width = 95
      Height = 38
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 13
      OnClick = BtnOKClick
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C1C9CDB1B9BBA7AFB1A1A9AB9FA6A9A2A9ABA8AFB2B2B8BCC2CACEC8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C3C9CDB3B8BBAAAFB1A4A8
        AAA2A6A8A4A9ABABAFB2B4B9BBC3CACEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BFC7
        CAA2B0AC68886D417242316A2E2A65262D632B39643A49654C57665C757D7EAB
        B2B5C2CACEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C1C7CBAAAFB17B7C7D5E5E5E5151514B4B
        4B4B4B4B5252525A5A5A6060607A7C7CADB2B4C3CACDC8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C5CDD1A4B5AE4B84
        4C25761924780F2277082274032571012770042A6E09286B102265163163324F
        5E54747A7CB0B7BAC5CDD1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C6CDD1ADB2B46E6F6F5151514E4E4E4C4C4C4848484646
        464747474848484848484545454E4E4E595959787979B2B7B9C6CED1C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C5CDD168936D1C821C078B
        05049000059100079100088E000E8800188000287400306D002E6A00266B0724
        661C3B5A405D5F628B9094C6CDD1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C6CDD18384855757575353535454545555555555555454545151
        514D4D4D4848484444444343434444444848484E4E4E5F5F5F8F9091C6CED1C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C5CED1629568148915009300019A
        01009A01009801009701009600009300048F000C8A00168200297400326A0028
        6C04226715335B375D5E62B0B7BAC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4A0A4A65959595555555A5A5A5A5A5A5959595959595858585656
        565555555252524E4E4E4848484444444444444646464B4B4B5F5F5FB2B7B9C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D49BB8A4128A15019905029F07029D
        07049E0A0AA6190CA81E059E0D0097010097000096000093000E870024760033
        6C00276C012267153B5C40757A7CC2CACDC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4B6BCBF6768695B5B5B5F5F5F5E5E5E6060606969696C6C6C6161615959
        59585858585858555555505050494949454545434343464646505050787A7AC3
        CACDC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BAC9C5369739099F1107A21006A00F08A3
        142CB74273D18A85D99C44C35E08A414009A02009701009600009400088B0025
        7600336B00286C0324681C4E5E54AAB1B5C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C1C8CB727374636363646464636363666666848484B0B0B0BCBCBC9696966666
        665A5A5A595959585858565656525252494949444444444444494949595959AD
        B2B4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C5CED159A3601CA4250DA61A0BA41907A51830B6
        45A3DEB2EEF2EFFDF9FCC2EED34BC86806A415019A040199020096010094000A
        8C00267600346900266B062F6431757D7CC2CACEC8D0D4C8D0D4C8D0D4C8D0D4
        A4A9AB6E6E6E6A6A6A686868686868858585CACACAF0F0F0FAFAFAE0E0E09C9C
        9C6767675B5B5B5A5A5A5858585757575353534949494343434545454E4E4E7A
        7C7CC3CACEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4B1C5BC279B2F24B13411AA2304A51829B33BA0D7
        ABEFEEF0F6F3F5F8F6F7FBFBFBCBF1D848C96905A517009A0302990300970100
        95001086002D73002D6B0021671657665CB2B8BCC8D0D4C8D0D4C8D0D4C2C9CC
        7A7C7C7B7B7B7070706868687E7E7EC3C3C3EFEFEFF3F3F3F6F6F6FBFBFBE5E5
        E59D9D9D6868685B5B5B5A5A5A58585858585850505048484844444446464661
        6161B4B9BBC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D49DBEA82EAB3B24B3380AAA1E17A9268ECD95ECEA
        EBF1EEF0F6F3F5FBF6FAFDF9FCFFFEFFC0F0D244C76504A517009A03019A0300
        97000393001A8100346C00296C0F48664BA8B0B2C8D0D4C8D0D4C8D0D4B3B8BA
        7B7B7B7D7D7D6D6D6D717171B4B4B4EAEAEAEEEEEEF3F3F3F7F7F7FAFAFAFEFE
        FEE1E1E19A9A9A6868685B5B5B5B5B5B5858585656564E4E4E4444444848485A
        5A5AABAFB2C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D48CB9973CB84E25B53C12A71F73C078E2E2E1F2EA
        F1F2EEF1E4ECE4E8F0E9F7F7F7FDFCFDFEFEFEB2EDC944C76506A617019B0501
        99020099000E8A002F71002B7009386539A2A9ABC8D0D4C8D0D4C8D0D4ABB0B2
        8989898080806C6C6CA1A1A1E1E1E1EDEDEDEFEFEFE9E9E9EDEDEDF6F6F6FCFC
        FCFEFEFEDBDBDB9A9A9A6868685C5C5C5A5A5A59595952525247474749494952
        5252A4A9ABC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D481B78A45BE5A2BB54136A535DEDDDCF2E7F2ECE8
        ECD1E2D184CE8E80CE88C3E7C9F8FAF9FDFCFDFEFEFEC0F0D248C96907A51701
        9C05009B01089100207D002771042C642A9FA6A9C8D0D4C8D0D4C8D0D4A6ABAD
        929292828282767676DDDDDDECECECE9E9E9DADADAB2B2B2AFAFAFDADADAF9F9
        F9FCFCFCFEFEFEE1E1E19D9D9D6868685D5D5D5A5A5A5656564D4D4D4848484C
        4C4CA2A6A8C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D47DB7874DC46536BD4F28A12BB1CBAFEFE5EDC3D9
        C355BC5F16B1320FAD254DC160C2E7C8FAFBFAFFFDFEFCFDFDC9F1D74AC76807
        A416009A02029500158700257301296624A1A8ABC8D0D4C8D0D4C8D0D4A6ABAD
        9A9A9A8C8C8C6F6F6FC0C0C0E8E8E8D0D0D0949494797979727272969696D9D9
        D9FAFAFAFDFDFDFDFDFDE4E4E49C9C9C6767675A5A5A5757575151514747474B
        4B4BA4A8AAC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D481B98C58C9714AC86711AC2A41AE4B7CBE7E4BB6
        531AB4351DB9411EB94114B2324DC15FC3E7C8FBFAFAFFFCFFFBFCFCC9EFD64A
        C66707A4140097010F8D00257602306C2DA7AFB1C8D0D4C8D0D4C8D0D4A8ADAF
        A1A1A19C9C9C737373838383A3A3A38C8C8C7B7B7B8383838383837979799696
        96D9D9D9FAFAFAFDFDFDFCFCFCE3E3E39B9B9B6767675959595454544A4A4A52
        5252AAAFB1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D48FBD9A5CC97461D17E2BC45514B43704A51B10B0
        2F25C04E26BE4D22BB471FB9420EB02D50C161C6E6CBFBF8FBFDF9FDFDFBFDBB
        ECCE43C56107A313098E00267807407341B0B8BBC8D0D4C8D0D4C8D0D4AEB2B5
        A3A3A3ABABAB9191917B7B7B6A6A6A7777778C8C8C8A8A8A8686868484847676
        76979797DADADAF9F9F9FAFAFAFBFBFBDDDDDD9797976666665454544C4C4C5E
        5E5EB3B8BBC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4A1C3AC55C36A6DD68A39C8612CC65D2EC8602EC7
        5D29C25527C05125BE4D23BD481FB8410AAC2259C266C8E5CCF9F7F9F9F6F8FD
        F9FCAEE8C42DBD4C048F0127790E67896CC1C9CDC8D0D4C8D0D4C8D0D4B6BBBE
        9B9B9BB3B3B39999999595959797979595959090908D8D8D8A8A8A8888888383
        837070709A9A9ADADADAF7F7F7F6F6F6FAFAFAD6D6D68A8A8A5454545050507B
        7C7DC2C9CDC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4B3C9BE45B7556ED88D5FD48433C9632DC85E2FC8
        602CC55C2BC45728C15224BD4C21BA451EB63E08AC214ABD5AC4E2C7F7F2F7FC
        F5FBCCEDDB57C66C049101257918A2B0ABC8D0D4C8D0D4C8D0D4C8D0D4C4CBCF
        979899B4B4B4AEAEAE9999999696969797979494949292928E8E8E8989898585
        858181816F6F6F929292D6D6D6F4F4F4F7F7F7E3E3E39D9D9D555555525252A9
        AEB0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C5CFD160B86B61D1807BDE9D4ED27A33CB6530CA
        642FC8622DC65E2BC45727C05022BB481FB84118B3370BAC2544BA55B4DCBACB
        E6D07CD2911CAE2C0B89054B874CBFC7CAC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        B1B5B8ABABABBFBFBFA8A8A89A9A9A9A9A9A9898989595959191918D8D8D8787
        878282827C7C7C7171718E8E8ECDCDCDDCDCDCB5B5B57777775353536F7071C1
        C7CBC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BBCCC756B96770DA9376DE994BD27732CB
        652FCA6330C9622DC65C29C25324BD4A20B9431CB53D15B13110AC282DB33F38
        B74A16AC2604980920831DA4B5AEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C4CBCF969798B8B8B8BDBDBDA7A7A79B9B9B9999999898989494948F8F8F8989
        898383837F7F7F7878787272728181818787877373735C5C5C595959AEB2B5C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4A8C6B260C37571DA9274DD984AD2
        7835CC6830CA632CC65D28C15424BD4C21BA441DB63D18B13515AE2C0FAA2209
        A619049D0D168D1A66956DC5CDD1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4BCC3C6A0A2A2B8B8B8BCBCBCA7A7A79C9C9C9999999595958F8F8F8989
        898484848080807A7A7A7575756F6F6F6969696060605D5D5D838485C6CED1C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D48FBD9760C2756FDA9279DE
        9C5AD58338CD692DC85C29C25525BE4C20B9431CB53C19B23516AF2E12AC260B
        A41C138F1A629868C5CDD1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4B1B6B9A0A2A2B8B8B8BEBEBEAEAEAE9D9D9D9595959090908A8A
        8A8484847F7F7F7B7B7B7676767272726A6A6A6C6D6EA1A6A8C6CDD1C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4A8C6B258BB6864D3
        8370DB946CDA8F59D27E42CA6830C25625BC471FB73E1DB4391DB23418A72734
        9B3B9BB9A4C5CED1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4BCC3C697999AAEAEAEB9B9B9B6B6B6ABABAB9D9D9D9090
        908787878181817E7E7E7B7B7B707070757676B7BDC0C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BBCCC762B9
        6E49BC5D59C8745ECE7D58CD764FC86B47C3623EBB5430AE4028A03258A560BA
        C9C5C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C4CBCFB2B7B99C9D9EA2A2A2A8A8A8A5A5A59E9E
        9E9797978E8E8E7E7E7E7E7F80A6AAADC2C8CCC8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C5CF
        D1B3CABFA2C3AD90BE9C85BC9083BB8E84B98E8FBA999FBFA9B1C5BCC5CED1C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C5CCCFB7BDBFB0B4B7ACB0B3AAAF
        B1A9AEB0ACB1B3B3B8BBC3C9CDC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4}
      NumGlyphs = 2
    end
    object edNumMac: TMaskEdit
      Tag = 2
      Left = 58
      Top = 15
      Width = 27
      Height = 21
      EditMask = '!99;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      MaxLength = 2
      ParentFont = False
      TabOrder = 1
      Text = '00'
    end
    object cbTipoMac: TComboBox
      Left = 59
      Top = 43
      Width = 138
      Height = 21
      Style = csDropDownList
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 3
      Items.Strings = (
        'CARICO'
        'SCARICO')
    end
    object edTipo: TMaskEdit
      Tag = 2
      Left = 169
      Top = 15
      Width = 27
      Height = 21
      EditMask = '!999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      MaxLength = 3
      ParentFont = False
      TabOrder = 2
      Text = '000'
    end
    object txtEdit: TEdit
      Left = 9
      Top = 210
      Width = 210
      Height = 21
      TabOrder = 8
    end
    object BtnCancel: TBitBtn
      Left = 118
      Top = 280
      Width = 95
      Height = 38
      Caption = 'Cancel'
      Default = True
      ModalResult = 2
      TabOrder = 14
      OnClick = BtnOKClick
      Glyph.Data = {
        0E100000424D0E100000000000003600000028000000340000001A0000000100
        180000000000D80F0000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C7CFD3C4CCD0BCC4C8B4BBBEAEB5B9ACB3B6ACB3B6
        AEB5B9B4BBBEBCC4C8C4CCD0C7CFD3C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0CFCFCFCCCCCCC4
        C4C4BBBBBBB5B5B5B3B3B3B3B3B3B5B5B5BBBBBBC4C4C4CCCCCCCFCFCFD0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C4CCD0B8C0C3A4AAAE8B909373787A64686A5D61635D616364686A74
        797B8B9093A5ABAFB8C0C3C4CCD0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0CCCCCCBFBFBFAAAAAA9090907878
        78686868616161616161686868797979909090ABABABBFBFBFCCCCCCD0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C7CFD3C0C8CC
        A9AFB37F848D545885353A8E1B219B0D13A20D129F1B1F95343683494C6F4E52
        5660646583888BA9B0B4C0C8CCC7CFD3C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0
        D0D0D0D0D0D0D0D0CFCFCFC8C8C8AFAFAF8686866464645252524444443D3D3D
        3B3B3B4141414C4C4C565656525252636363888888B0B0B0C8C8C8CFCFCFD0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C7CFD3BEC5C99BA1A84F56980C
        17AF0014C10019CA0018C90014C30012BF0012BE000FBA000AAD0D109B434578
        4D50526E72749EA5A8BFC6CAC7CFD3C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0
        D0CFCFCFC5C5C5A2A2A26868684343434545454B4B4B4A4A4A46464643434343
        4343404040393939393939545454505050727272A5A5A5C6C6C6CFCFCFD0D0D0
        D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C0C8CC9097AC2531AA001ED70029EC002A
        EC0028EB0028E90026E50021DC001ACE0012BD000FB8000EB80009AB24268E49
        4C54676B6D9EA4A7C0C8CCC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0C8C8C8
        9C9C9C5353535151515E5E5E5F5F5F5D5D5D5D5D5D5A5A5A5555554C4C4C4343
        433F3F3F3F3F3F3838384444444E4E4E6B6B6BA4A4A4C8C8C8D0D0D0D0D0D0D0
        D0D0C8D0D4C8D0D4C4CCD099A1B71827B9002CF2002DF7002DF5002DF3002CF1
        002AEF0029EC0027E80025E70023E3001BD10010B7000DB4000DB3181A944A4D
        566D7174A9B0B4C4CCD0C8D0D4C8D0D4D0D0D0D0D0D0CCCCCCA6A6A650505062
        62626464646363636363636161616060605E5E5E5C5C5C5A5A5A5858584E4E4E
        4040403D3D3D3D3D3D3E3E3E4F4F4F717171B0B0B0CCCCCCD0D0D0D0D0D0C8D0
        D4C7CFD3B3BAC42535B80232FA0034FE073BFE063AFE002EFE0030FB002FF900
        2DF6002CF10025ED0F3AF0001FE4001EDE0011BD000DB4000DB323268E4D5052
        83888BB8C0C3C7CFD3C8D0D4D0D0D0CFCFCFBCBCBC5A5A5A6868686A6A6A6F6F
        6F6E6E6E6666666767676666666363636161615C5C5C6B6B6B56565653535342
        42423D3D3D3D3D3D444444505050888888BFBFBFCFCFCFD0D0D0C8D0D4C4CCD0
        5D69BE1038EE0538FE0338FEB3BDE09BADEA2C5DFE0338FE0033FE0032FE002A
        F92E5FFDA9C0FD4B78FE204EF5001DE10011BB000DB5000AAC424578606465A4
        AAAEC4CCD0C8D0D4D0D0D0CCCCCC8181816969696D6D6D6D6D6DC6C6C6BDBDBD
        8787876D6D6D696969696969636363888888CFCFCF9A9A9A7A7A7A5454544141
        413D3D3D393939535353636363AAAAAACCCCCCD0D0D0C8D0D4B1B9C8112CD324
        54FE0539FE4567F1F0EAD5E7E5DDE8E6E15E84F90B3FFE0031FE2F61FED3DEF9
        FEFEF9FEFEFCC0D2FE1A49F3001FDE0010B6000FB90C0F9C4E52578A9092BCC4
        C8C8D0D4D0D0D0BCBCBC5A5A5A8181816D6D6D8C8C8CE4E4E4E2E2E2E4E4E4A2
        A2A27272726868688A8A8AE4E4E4FCFCFCFDFDFDDDDDDD7676765454543F3F3F
        4040403838385353538F8F8FC4C4C4D0D0D0C8D0D46F7CC3284DF11E50FE1244
        FE0028FEBBC2E0E1E1DEE6E6E3F8F5E67D9BF81E54FED4DDF8FEFEF9FEFEFEFE
        FEFEFEFEFE89AAFE0026E8001ACF000FB9000BB1484B6F73787AB4BBBEC8D0D4
        D0D0D08F8F8F7979797E7E7E757575636363CACACAE0E0E0E5E5E5F0F0F0B3B3
        B3808080E4E4E4FCFCFCFEFEFEFEFEFEFEFEFEBFBFBF5B5B5B4D4D4D4040403B
        3B3B555555787878BBBBBBD0D0D0C8D0D43C4CC74D75FE1E4FFE1A4DFE002AFA
        E2E0DBE8E6DFE5E5E4EBEBE9F9F6EEDBE0F4FEFDF7FCFBFBFEFEFEFEFEFEDEE7
        FE4C78FE022BED0025E50012BB0012BC33368764686AAEB5B9C8D0D4D0D0D06E
        6E6E9999997D7D7D7C7C7C636363DEDEDEE4E4E4E4E4E4EAEAEAF3F3F3E5E5E5
        FBFBFBFBFBFBFEFEFEFEFEFEECECEC9B9B9B6060605A5A5A4242424242424D4D
        4D686868B5B5B5D0D0D0C8D0D41C38D55E83FE2354FE2051FE1748FB0A36F4AB
        B7E7F7F3E5EDEDEAF0F0EFF7F7F4F8F8F8FEFEFEFEFEFEB6CBFE2256FE0029F8
        002CF1002AEC0019CD0012C0191D9C5C5F61ACB3B6C8D0D4D0D0D0636363A3A3
        A38181817F7F7F777777696969C3C3C3EFEFEFECECECEFEFEFF6F6F6F8F8F8FE
        FEFEFEFEFED7D7D78282826262626161615F5F5F4B4B4B4343434242425F5F5F
        B3B3B3D0D0D0C8D0D41538E0678BFE2F5DFE2758FE295BFE2758FE0032FC4E6E
        F1ECEBEBF0F0EFF4F4F4F9F9F9FEFEFE8FABFE184BFE0032FE0032FE002EF700
        2AF00020DA0014C20A13A75D6062ACB3B6C8D0D4D0D0D0666666A9A9A9878787
        848484868686848484686868919191EBEBEBEFEFEFF4F4F4F9F9F9FEFEFEC0C0
        C07A7A7A6969696969696464646060605353534545453E3E3E606060B3B3B3D0
        D0D0C8D0D4193BE16E90FE416DFE2D5DFE2F5FFE3161FE3263FE0028FBCFD4ED
        F2F2EFF4F4F4FAF9F9FEFEFE6186FE093EFE093BFE0336FE0031FB002CF30024
        E50016C60A14A964686AAEB5B9C8D0D4D0D0D0686868ADADAD93939387878789
        89898A8A8A8B8B8B626262DADADAF1F1F1F4F4F4F9F9F9FEFEFEA5A5A5717171
        6F6F6F6B6B6B6767676262625959594848483F3F3F686868B5B5B5D0D0D0C8D0
        D42442DE7595FE5A80FE3263FE3766FE3867FE2D5EFE435FECFBF8EAF0F0EEFD
        FBF3FEFEF8FEFEFEFEFEFE3866FE083BFE0639FE0133FE002EF7002AEC0018CB
        1A23A1757A7CB4BBBEC8D0D4D0D0D06D6D6DB0B0B0A1A1A18B8B8B8E8E8E8E8E
        8E888888858585F4F4F4EFEFEFF8F8F8FCFCFCFEFEFEFEFEFE8E8E8E6F6F6F6E
        6E6E6969696464645F5F5F4A4A4A4747477A7A7ABBBBBBD0D0D0C8D0D44055D8
        7295FE7B9AFE3769FE3E6DFE4171FE0A37F1BDC3E5EEEDE8FDFAED6B85F7A8B8
        F9FEFEFBFEFEFEE2E8FE1349FE0638FE0335FE0030FA002AEF001ACD363D928C
        9194BCC4C8C8D0D4D0D0D0797979B0B0B0B4B4B48F8F8F929292959595696969
        CCCCCCEBEBEBF6F6F6A4A4A4C9C9C9FDFDFDFEFEFEEDEDED7878786D6D6D6B6B
        6B6666666060604C4C4C555555919191C4C4C4D0D0D0C8D0D47A8AD74F71F686
        A6FE5881FE4473FE4779FE1736DFF7F3E2EFEDE7C0C7EC0030F9002FFDB2C0FA
        FEFEFCFEFEFE93ABFE093EFE0437FE0033FE002BEF0017C8555B87A5ACAFC5CD
        D1C8D0D4D0D0D09F9F9F949494BCBCBCA1A1A19797979B9B9B646464EEEEEEEB
        EBEBD1D1D1666666676767CFCFCFFDFDFDFEFEFEC1C1C17171716C6C6C696969
        606060494949676767ACACACCDCDCDD0D0D0C8D0D4BEC6D52348E989AAFE89A7
        FE4A79FE305BF47481D8F6F3E1F7F5E51138EA3464FE3061FE002DFCC7D1FAFE
        FEFCFEFEFE4A73FE0538FE0033FE002AED0F1EB781878FB8C0C3C8D0D4C8D0D4
        D0D0D0C9C9C9737373BFBFBFBDBDBD9B9B9B838383999999EDEDEDF0F0F06868
        688C8C8C8A8A8A656565DCDCDCFDFDFDFEFEFE9797976D6D6D6969695F5F5F4A
        4A4A888888BFBFBFD0D0D0D0D0D0C8D0D4C8D0D46B81E2597CF78FAEFE7D9FFE
        4172FE0C2AD78B95DA7080DE1F4EF73B6AFE3262FE2859FE002DFBE1E5F9B7C5
        FB436BFE0C3FFE0136FE0020D9545C9CABB2B5C5CDD1C8D0D4C8D0D4D0D0D0D0
        D0D09B9B9B9C9C9CC2C2C2B7B7B79696965A5A5AA8A8A89A9A9A7B7B7B909090
        8B8B8B848484656565EAEAEAD3D3D39292927272726B6B6B5353536E6E6EB2B2
        B2CDCDCDD0D0D0D0D0D0C8D0D4C8D0D4C5CDD52E52E47A9AFD90ADFE7C9EFE48
        78FE2751F11038E84575FE3A69FE3363FE2B5CFE2152FE002AFC0136FE0B3EFE
        083CFE0131F82B3AB19FA6ABC1C9CDC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0CECE
        CE797979B3B3B3C1C1C1B6B6B69A9A9A7C7C7C6868689898989090908B8B8B86
        86867F7F7F6464646B6B6B7171717070706767675B5B5BA6A6A6C9C9C9D0D0D0
        D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4B5BFD7264BE87999FD8EADFE88A6FE5D87
        FE4876FE3F6FFE3968FE3262FE2B5BFE2355FE1D50FE1647FE0F43FE0738FA1F
        33BD979EAFBFC6CAC7CFD3C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0C5C5C5
        757575B3B3B3C1C1C1BCBCBCA5A5A59999999494948F8F8F8B8B8B8686868181
        817E7E7E7878787474746C6C6C595959A2A2A2C6C6C6CFCFCFD0D0D0D0D0D0D0
        D0D0C8D0D4C8D0D4C8D0D4C8D0D4B7C1D73154E45578F88CADFE8CABFE7C9EFE
        5A83FE4470FE3160FE2757FE2153FE2253FE2354FE0D35EC2F43BAA1A8BAC1C9
        CDC7CFD3C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0C6C6C67B
        7B7B9A9A9AC1C1C1C0C0C0B6B6B6A3A3A3959595898989838383808080808080
        818181676767646464ACACACC9C9C9CFCFCFD0D0D0D0D0D0D0D0D0D0D0D0C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C6CED57286E22348E94D71F57497FE7A9DFE74
        96FE6B90FE6087FE4972FD254AEF1734D46977C2B8C0C5C5CDD1C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0CFCFCF9F9F
        9F737373949494B1B1B1B6B6B6B1B1B1ACACACA6A6A69696967777776060608B
        8B8BC0C0C0CDCDCDD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C1CAD58393D74B5FDC2A4BDF2244E42240
        E02844D7495BCD7D8BC7B7BECAC5CDD1C7CFD3C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0CCCCCC
        A5A5A58282827373736F6F6F6C6C6C6C6C6C7B7B7B9B9B9BC0C0C0CDCDCDCFCF
        CFD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      NumGlyphs = 2
    end
    object RadioButton1: TRadioButton
      Left = 9
      Top = 235
      Width = 110
      Height = 17
      Caption = 'Radio Button 1'
      Checked = True
      TabOrder = 9
      TabStop = True
    end
    object RadioButton2: TRadioButton
      Left = 9
      Top = 252
      Width = 110
      Height = 17
      Caption = 'Radio Button 2'
      TabOrder = 11
    end
    object Check1: TCheckBox
      Left = 109
      Top = 235
      Width = 110
      Height = 17
      Caption = 'Check Box 1'
      TabOrder = 10
    end
    object Check2: TCheckBox
      Left = 109
      Top = 252
      Width = 110
      Height = 17
      Caption = 'Check Box 2'
      TabOrder = 12
    end
    object edDataStart: TMaskEdit
      Tag = 6
      Left = 60
      Top = 75
      Width = 67
      Height = 21
      EditMask = '!99/99/9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 10
      ParentFont = False
      TabOrder = 4
      Text = '01/01/2000'
    end
    object edOraStart: TMaskEdit
      Tag = 7
      Left = 130
      Top = 75
      Width = 67
      Height = 21
      EditMask = '!99.99.99;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 8
      ParentFont = False
      TabOrder = 5
      Text = '  .  .  '
    end
    object edDataEnd: TMaskEdit
      Tag = 6
      Left = 60
      Top = 130
      Width = 67
      Height = 21
      EditMask = '!99/99/9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 10
      ParentFont = False
      TabOrder = 6
      Text = '01/01/2000'
    end
    object edOraEnd: TMaskEdit
      Tag = 7
      Left = 130
      Top = 130
      Width = 67
      Height = 21
      EditMask = '!99.99.99;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 8
      ParentFont = False
      TabOrder = 7
      Text = '  .  .  '
    end
  end
  object DateTimePickerStart: TDateTimePicker
    Left = 7
    Top = 100
    Width = 190
    Height = 21
    CalAlignment = dtaLeft
    Date = 42608.6112805787
    Time = 42608.6112805787
    DateFormat = dfLong
    DateMode = dmComboBox
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Kind = dtkDate
    ParseInput = False
    ParentFont = False
    TabOrder = 1
    OnChange = DateTimePickerSEChange
  end
  object DateTimePickerEnd: TDateTimePicker
    Tag = 1
    Left = 7
    Top = 155
    Width = 190
    Height = 21
    CalAlignment = dtaLeft
    Date = 42608.6112805787
    Time = 42608.6112805787
    DateFormat = dfLong
    DateMode = dmComboBox
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Kind = dtkDate
    ParseInput = False
    ParentFont = False
    TabOrder = 2
    OnChange = DateTimePickerSEChange
  end
end
