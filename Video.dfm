object FormVideo: TFormVideo
  Left = 263
  Top = 185
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Presentazione impianto'
  ClientHeight = 344
  ClientWidth = 323
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
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
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object VideoPanel: TPanel
    Left = 0
    Top = 0
    Width = 323
    Height = 344
    Align = alClient
    TabOrder = 0
    object Label1: TLabel
      Left = 69
      Top = 255
      Width = 188
      Height = 13
      Caption = 'GRUPPO TECNOFERRARI s.p.a.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label2: TLabel
      Left = 14
      Top = 280
      Width = 296
      Height = 13
      Caption = 'via Ghiarola Vecchia - 41042 Fiorano Modenese (MO) - ITALY '
    end
    object Label3: TLabel
      Left = 35
      Top = 300
      Width = 252
      Height = 13
      Caption = 'Telefono: +39 0536/915000 - Fax: +39 0536/915045'
    end
    object Label4: TLabel
      Left = 100
      Top = 320
      Width = 123
      Height = 13
      Caption = 'E-mail: info@tecnoferrari.it'
    end
    object Bevel1: TBevel
      Left = 5
      Top = 241
      Width = 316
      Height = 16
      Shape = bsTopLine
    end
    object PanelLogo: TPanel
      Left = 43
      Top = 15
      Width = 236
      Height = 209
      BevelInner = bvLowered
      TabOrder = 0
      object ImageLogo: TImage
        Left = 2
        Top = 2
        Width = 232
        Height = 205
        Align = alClient
        Transparent = True
      end
    end
    object PresentaMediaPlayer: TMediaPlayer
      Left = 260
      Top = 205
      Width = 57
      Height = 30
      ColoredButtons = []
      EnabledButtons = [btPlay, btStop]
      VisibleButtons = [btPlay, btStop]
      DeviceType = dtAVIVideo
      Display = VideoPanel
      FileName = 'D:\Clienti\TecnoFerrari\Box2000\Clai\Avi\porco.avi'
      Visible = False
      TabOrder = 1
    end
  end
end
