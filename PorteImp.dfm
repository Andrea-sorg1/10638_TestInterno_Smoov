object FormPorteImp: TFormPorteImp
  Left = 61
  Top = 114
  BorderStyle = bsDialog
  Caption = '*Porte impianto'
  ClientHeight = 427
  ClientWidth = 303
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
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object gbNodi: TGroupBox
    Left = 0
    Top = 5
    Width = 301
    Height = 101
    Caption = '*Nodi in ingombro alle porte:'
    TabOrder = 0
    object Label1: TLabel
      Left = 10
      Top = 28
      Width = 14
      Height = 13
      Caption = '01'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label2: TLabel
      Left = 10
      Top = 53
      Width = 14
      Height = 13
      Caption = '02'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label3: TLabel
      Left = 65
      Top = 28
      Width = 14
      Height = 13
      Caption = '03'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label4: TLabel
      Left = 65
      Top = 53
      Width = 14
      Height = 13
      Caption = '04'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label5: TLabel
      Left = 120
      Top = 28
      Width = 14
      Height = 13
      Caption = '05'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label6: TLabel
      Left = 120
      Top = 53
      Width = 14
      Height = 13
      Caption = '06'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label7: TLabel
      Left = 175
      Top = 28
      Width = 14
      Height = 13
      Caption = '07'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label8: TLabel
      Left = 175
      Top = 53
      Width = 14
      Height = 13
      Caption = '08'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label9: TLabel
      Left = 230
      Top = 28
      Width = 14
      Height = 13
      Caption = '09'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label10: TLabel
      Left = 230
      Top = 53
      Width = 14
      Height = 13
      Caption = '10'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblAnticipoRich: TLabel
      Left = 10
      Top = 78
      Width = 102
      Height = 13
      Caption = '*anticipo richiesta'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblInterbloc: TLabel
      Left = 150
      Top = 78
      Width = 112
      Height = 13
      Caption = '*porta interbloccata'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object EditNodo1: TMaskEdit
      Tag = 2
      Left = 29
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 0
      Text = '0000'
    end
    object EditNodo2: TMaskEdit
      Tag = 2
      Left = 29
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 1
      Text = '0000'
    end
    object EditNodo3: TMaskEdit
      Tag = 2
      Left = 84
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 2
      Text = '0000'
    end
    object EditNodo4: TMaskEdit
      Tag = 2
      Left = 84
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 3
      Text = '0000'
    end
    object EditNodo5: TMaskEdit
      Tag = 2
      Left = 139
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 4
      Text = '0000'
    end
    object EditNodo6: TMaskEdit
      Tag = 2
      Left = 139
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 5
      Text = '0000'
    end
    object EditNodo7: TMaskEdit
      Tag = 2
      Left = 194
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 6
      Text = '0000'
    end
    object EditNodo8: TMaskEdit
      Tag = 2
      Left = 194
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 7
      Text = '0000'
    end
    object EditNodo9: TMaskEdit
      Tag = 2
      Left = 249
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 8
      Text = '0000'
    end
    object EditNodo10: TMaskEdit
      Tag = 2
      Left = 249
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 9
      Text = '0000'
    end
    object EdAnticipo: TMaskEdit
      Tag = 2
      Left = 114
      Top = 74
      Width = 22
      Height = 21
      EditMask = '!99;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 2
      ParentFont = False
      TabOrder = 10
      Text = '00'
    end
    object EdInterblocco: TMaskEdit
      Tag = 2
      Left = 264
      Top = 74
      Width = 22
      Height = 21
      EditMask = '!99;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 2
      ParentFont = False
      TabOrder = 11
      Text = '00'
    end
    object CBEsclusa: TCheckBox
      Left = 170
      Top = -2
      Width = 111
      Height = 17
      Caption = '*Escludi PLC'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 12
    end
  end
  object gbConsensi: TGroupBox
    Left = 0
    Top = 115
    Width = 161
    Height = 156
    Caption = '*Stato consensi:'
    Enabled = False
    TabOrder = 1
    object CBRich: TCheckBox
      Left = 10
      Top = 15
      Width = 116
      Height = 17
      Caption = '*Rich. ingresso'
      TabOrder = 0
    end
    object CBOk: TCheckBox
      Left = 10
      Top = 35
      Width = 116
      Height = 17
      Caption = '*Ok accesso'
      TabOrder = 1
    end
    object CBIng: TCheckBox
      Left = 10
      Top = 75
      Width = 116
      Height = 17
      Caption = '*Tgv ingombro'
      TabOrder = 2
    end
    object CBOkPar: TCheckBox
      Left = 10
      Top = 55
      Width = 116
      Height = 17
      Caption = '*Apertura parziale'
      TabOrder = 3
    end
    object CBCarico: TCheckBox
      Left = 10
      Top = 95
      Width = 116
      Height = 17
      Caption = '*TGV con carico'
      TabOrder = 4
    end
    object CBBlocco: TCheckBox
      Left = 10
      Top = 115
      Width = 116
      Height = 17
      Caption = '*Porta in blocco'
      TabOrder = 5
    end
    object CBTgvInArrivo: TCheckBox
      Left = 10
      Top = 135
      Width = 116
      Height = 17
      Caption = '*Tgv in arrivo'
      TabOrder = 6
    end
  end
  object BtnOk: TBitBtn
    Left = 0
    Top = 386
    Width = 301
    Height = 38
    Caption = '*OK'
    Default = True
    TabOrder = 2
    OnClick = BtnOkClick
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
  object gbDirezione: TGroupBox
    Left = 165
    Top = 115
    Width = 136
    Height = 156
    Caption = '*Direzione:'
    TabOrder = 3
    object CBUp: TCheckBox
      Left = 5
      Top = 20
      Width = 46
      Height = 17
      Caption = '*UP'
      TabOrder = 0
    end
    object CBDown: TCheckBox
      Left = 5
      Top = 40
      Width = 76
      Height = 17
      Caption = '*DOWN'
      TabOrder = 1
    end
    object CBLeft: TCheckBox
      Left = 5
      Top = 60
      Width = 76
      Height = 17
      Caption = '*LEFT'
      TabOrder = 2
    end
    object CBRight: TCheckBox
      Left = 5
      Top = 80
      Width = 76
      Height = 17
      Caption = '*RIGHT'
      TabOrder = 3
    end
  end
  object gbSemafori: TGroupBox
    Left = 2
    Top = 275
    Width = 301
    Height = 81
    Caption = '*Nodi semaforo porte:'
    TabOrder = 4
    object Label12: TLabel
      Left = 10
      Top = 28
      Width = 14
      Height = 13
      Caption = '01'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label13: TLabel
      Left = 10
      Top = 53
      Width = 14
      Height = 13
      Caption = '02'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label14: TLabel
      Left = 65
      Top = 28
      Width = 14
      Height = 13
      Caption = '03'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label15: TLabel
      Left = 65
      Top = 53
      Width = 14
      Height = 13
      Caption = '04'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label16: TLabel
      Left = 120
      Top = 28
      Width = 14
      Height = 13
      Caption = '05'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label17: TLabel
      Left = 120
      Top = 53
      Width = 14
      Height = 13
      Caption = '06'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label18: TLabel
      Left = 175
      Top = 28
      Width = 14
      Height = 13
      Caption = '07'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label19: TLabel
      Left = 175
      Top = 53
      Width = 14
      Height = 13
      Caption = '08'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label20: TLabel
      Left = 230
      Top = 28
      Width = 14
      Height = 13
      Caption = '09'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label21: TLabel
      Left = 230
      Top = 53
      Width = 14
      Height = 13
      Caption = '10'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object EditNsem1: TMaskEdit
      Tag = 2
      Left = 29
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 0
      Text = '0000'
    end
    object EditNsem2: TMaskEdit
      Tag = 2
      Left = 29
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 1
      Text = '0000'
    end
    object EditNsem3: TMaskEdit
      Tag = 2
      Left = 84
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 2
      Text = '0000'
    end
    object EditNsem4: TMaskEdit
      Tag = 2
      Left = 84
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 3
      Text = '0000'
    end
    object EditNsem5: TMaskEdit
      Tag = 2
      Left = 139
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 4
      Text = '0000'
    end
    object EditNsem6: TMaskEdit
      Tag = 2
      Left = 139
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 5
      Text = '0000'
    end
    object EditNsem7: TMaskEdit
      Tag = 2
      Left = 194
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 6
      Text = '0000'
    end
    object EditNsem8: TMaskEdit
      Tag = 2
      Left = 194
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 7
      Text = '0000'
    end
    object EditNsem9: TMaskEdit
      Tag = 2
      Left = 249
      Top = 24
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 8
      Text = '0000'
    end
    object EditNsem10: TMaskEdit
      Tag = 2
      Left = 249
      Top = 49
      Width = 32
      Height = 21
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 9
      Text = '0000'
    end
  end
  object UpDwPort: TUpDown
    Left = 0
    Top = 360
    Width = 301
    Height = 26
    Min = 0
    Orientation = udHorizontal
    Position = 0
    TabOrder = 5
    Wrap = False
    OnClick = UpDwPortClick
  end
end
