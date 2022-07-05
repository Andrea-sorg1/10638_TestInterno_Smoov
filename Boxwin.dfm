object FormBox: TFormBox
  Left = 896
  Top = 196
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = '*BOX'
  ClientHeight = 607
  ClientWidth = 362
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
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object GBDati: TGroupBox
    Left = 5
    Top = 30
    Width = 351
    Height = 276
    TabOrder = 0
    object lblID: TLabel
      Left = 15
      Top = 20
      Width = 14
      Height = 13
      Caption = 'ID'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label1: TLabel
      Left = 165
      Top = 253
      Width = 20
      Height = 13
      Caption = '--->'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object EdIdBox: TMaskEdit
      Left = 42
      Top = 16
      Width = 33
      Height = 21
      Color = 16181982
      EditMask = '!9999;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 4
      ParentFont = False
      TabOrder = 0
      Text = '0000'
      OnClick = AzzeraTimerClick
    end
    object CBStatLav: TComboBox
      Left = 85
      Top = 16
      Width = 200
      Height = 21
      Style = csDropDownList
      Color = 16181982
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 1
      OnChange = CBStatLavChange
      OnClick = AzzeraTimerClick
      Items.Strings = (
        '0 - VUOTO'
        '1 - CRUDO'
        '2 - ESSICCATO')
    end
    object RGDati: TGroupBox
      Left = 5
      Top = 40
      Width = 341
      Height = 181
      Caption = '*dati presa:'
      TabOrder = 2
      object Bevel1: TBevel
        Left = 5
        Top = 50
        Width = 321
        Height = 11
        Shape = bsTopLine
      end
      object Bevel3: TBevel
        Left = 5
        Top = 83
        Width = 321
        Height = 11
        Shape = bsTopLine
      end
      object lblTipoPallet: TStaticText
        Tag = 1
        Left = 35
        Top = 27
        Width = 57
        Height = 17
        Alignment = taRightJustify
        Caption = '*Tipo pallet'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
      end
      object CBTipPal: TComboBox
        Left = 95
        Top = 25
        Width = 166
        Height = 21
        Style = csDropDownList
        Color = 11324151
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 13
        ParentFont = False
        TabOrder = 0
        OnClick = AzzeraTimerClick
        Items.Strings = (
          'V - VUOTO'
          'R - RESO'
          'F - FINITO'
          'S - SEMILAVORATO')
      end
      object CBBas: TCheckBox
        Left = 195
        Top = 131
        Width = 116
        Height = 17
        Caption = '*Pallet basso'
        TabOrder = 2
        OnClick = CBBasClick
      end
      object CBSovrapp: TCheckBox
        Left = 195
        Top = 91
        Width = 136
        Height = 17
        Caption = '*Pallet sovrapponibile'
        TabOrder = 3
      end
      object CBCarW: TCheckBox
        Left = 195
        Top = 111
        Width = 91
        Height = 17
        Caption = '*CarWash'
        TabOrder = 4
      end
      object gbConfigPresa: TGroupBox
        Left = 10
        Top = 91
        Width = 176
        Height = 71
        Caption = '*Configurazione presa:'
        TabOrder = 5
        object CBPos1B: TCheckBox
          Left = 10
          Top = 45
          Width = 46
          Height = 17
          Caption = '1 L'
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          TabOrder = 0
          OnClick = CBPosClick
        end
        object CBPos2B: TCheckBox
          Tag = 1
          Left = 65
          Top = 45
          Width = 46
          Height = 17
          Caption = '2 L'
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          TabOrder = 1
          OnClick = CBPosClick
        end
        object CBPos3B: TCheckBox
          Tag = 2
          Left = 120
          Top = 45
          Width = 46
          Height = 17
          Caption = '3 L'
          Color = clLime
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          TabOrder = 2
          OnClick = CBPosClick
        end
        object CBPos1A: TCheckBox
          Tag = 3
          Left = 10
          Top = 20
          Width = 46
          Height = 17
          Caption = '1 H'
          Color = clYellow
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          TabOrder = 3
          OnClick = CBPosClick
        end
        object CBPos2A: TCheckBox
          Tag = 4
          Left = 65
          Top = 20
          Width = 46
          Height = 17
          Caption = '2 H'
          Color = clYellow
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          TabOrder = 4
          OnClick = CBPosClick
        end
        object CBPos3A: TCheckBox
          Tag = 5
          Left = 120
          Top = 20
          Width = 46
          Height = 17
          Caption = '3 H'
          Color = clYellow
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          TabOrder = 5
          OnClick = CBPosClick
        end
      end
      object CBRot: TCheckBox
        Left = 195
        Top = 151
        Width = 116
        Height = 17
        Caption = '*Ruotato'
        TabOrder = 6
        OnClick = CBBasClick
      end
      object lblCod: TStaticText
        Tag = 1
        Left = 35
        Top = 59
        Width = 41
        Height = 17
        Alignment = taRightJustify
        Caption = '*Codice'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 7
      end
      object EdCod: TMaskEdit
        Tag = 1
        Left = 80
        Top = 57
        Width = 29
        Height = 21
        Color = clWhite
        EditMask = '!99;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 2
        ParentFont = False
        TabOrder = 8
        Text = '00'
        OnClick = AzzeraTimerClick
      end
      object lblLot: TStaticText
        Tag = 1
        Left = 125
        Top = 59
        Width = 32
        Height = 17
        Alignment = taRightJustify
        Caption = '*Lotto'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 9
      end
      object EdLot: TMaskEdit
        Tag = 1
        Left = 165
        Top = 57
        Width = 38
        Height = 21
        Color = clWhite
        EditMask = '!99999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 5
        ParentFont = False
        TabOrder = 10
        Text = '00000'
        OnClick = AzzeraTimerClick
      end
      object lblAlt: TStaticText
        Tag = 1
        Left = 220
        Top = 59
        Width = 42
        Height = 17
        Alignment = taRightJustify
        Caption = '*Altezza'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 11
      end
      object EdAltezza: TMaskEdit
        Tag = 1
        Left = 267
        Top = 57
        Width = 33
        Height = 21
        Color = clWhite
        EditMask = '!9999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 4
        ParentFont = False
        TabOrder = 12
        Text = '0000'
        OnClick = AzzeraTimerClick
      end
      object TxtLoc2: TStaticText
        Left = 280
        Top = 25
        Width = 56
        Height = 16
        Alignment = taCenter
        AutoSize = False
        BorderStyle = sbsSingle
        Caption = '0000000000'
        Font.Charset = ANSI_CHARSET
        Font.Color = clGray
        Font.Height = -9
        Font.Name = 'Small Fonts'
        Font.Style = []
        ParentFont = False
        TabOrder = 13
      end
      object TxtLoc1: TStaticText
        Left = 280
        Top = 10
        Width = 56
        Height = 16
        Alignment = taCenter
        AutoSize = False
        BorderStyle = sbsSingle
        Caption = '0000000000'
        Font.Charset = ANSI_CHARSET
        Font.Color = clGray
        Font.Height = -9
        Font.Name = 'Small Fonts'
        Font.Style = []
        ParentFont = False
        TabOrder = 14
      end
    end
    object CBGruppoSorg: TComboBox
      Left = 15
      Top = 249
      Width = 146
      Height = 21
      Style = csDropDownList
      Color = 16181982
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 3
      OnClick = AzzeraTimerClick
      Items.Strings = (
        '0 - VUOTO'
        '1 - CRUDO'
        '2 - ESSICCATO')
    end
    object CBGruppoDest: TComboBox
      Left = 190
      Top = 249
      Width = 146
      Height = 21
      Style = csDropDownList
      Color = 16181982
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 4
      OnClick = AzzeraTimerClick
      Items.Strings = (
        '0 - VUOTO'
        '1 - CRUDO'
        '2 - ESSICCATO')
    end
    object txtProv: TStaticText
      Left = 290
      Top = 10
      Width = 56
      Height = 16
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSingle
      Caption = '0000000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clGray
      Font.Height = -9
      Font.Name = 'Small Fonts'
      Font.Style = []
      ParentFont = False
      TabOrder = 5
    end
    object txtTime1: TStaticText
      Left = 17
      Top = 228
      Width = 141
      Height = 17
      Hint = 'Loading time'
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSingle
      Caption = 'txtTime1'
      TabOrder = 6
    end
    object txtTime2: TStaticText
      Left = 192
      Top = 228
      Width = 141
      Height = 17
      Hint = 'Monvement time'
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSingle
      Caption = 'txtTime2'
      TabOrder = 7
    end
  end
  object NRecordPanel: TPanel
    Left = 154
    Top = 5
    Width = 96
    Height = 26
    BevelInner = bvLowered
    Caption = 'NRecordPanel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
  end
  object EditPanel: TPanel
    Left = 260
    Top = 5
    Width = 91
    Height = 26
    BevelInner = bvLowered
    Caption = 'EDITAZIONE'
    TabOrder = 2
  end
  object GBUdc: TGroupBox
    Left = 5
    Top = 310
    Width = 351
    Height = 246
    TabOrder = 3
    object PgCUDC: TPageControl
      Left = 5
      Top = 10
      Width = 341
      Height = 231
      ActivePage = Tab06
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabIndex = 5
      TabOrder = 0
      OnChange = PgCUDCChange
      object Tab01: TTabSheet
        Caption = '01'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Tab02: TTabSheet
        Caption = '02'
        ImageIndex = 1
      end
      object Tab03: TTabSheet
        Caption = '03'
        ImageIndex = 2
      end
      object Tab04: TTabSheet
        Caption = '04'
        ImageIndex = 3
      end
      object Tab05: TTabSheet
        Caption = '05'
        ImageIndex = 4
      end
      object Tab06: TTabSheet
        Caption = '06'
        ImageIndex = 5
      end
    end
    object prdPanel: TPanel
      Left = 10
      Top = 35
      Width = 331
      Height = 201
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 1
      object txtSource: TStaticText
        Left = 5
        Top = 155
        Width = 60
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*Source'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
      end
      object edSize: TEdit
        Left = 5
        Top = 127
        Width = 70
        Height = 21
        AutoSize = False
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 10
        ParentFont = False
        TabOrder = 1
        Text = 'edSize'
      end
      object txtSize: TStaticText
        Left = 5
        Top = 110
        Width = 70
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*Size'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
      end
      object txtBatch: TStaticText
        Left = 85
        Top = 110
        Width = 70
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*Batch'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
      end
      object edBatch: TEdit
        Left = 85
        Top = 127
        Width = 70
        Height = 21
        AutoSize = False
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 10
        ParentFont = False
        TabOrder = 4
        Text = 'edBatch'
      end
      object txtDest: TStaticText
        Left = 80
        Top = 155
        Width = 60
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*Destination'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 5
      end
      object txtQuantity: TStaticText
        Left = 150
        Top = 155
        Width = 55
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*Quantity'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 6
      end
      object edState: TEdit
        Left = 165
        Top = 127
        Width = 70
        Height = 21
        AutoSize = False
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 4
        ParentFont = False
        TabOrder = 7
        Text = 'edSize'
      end
      object txtState: TStaticText
        Left = 165
        Top = 110
        Width = 70
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*State'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 8
      end
      object txtHeight: TStaticText
        Left = 210
        Top = 155
        Width = 55
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*Height'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 9
      end
      object txtWeight: TStaticText
        Left = 270
        Top = 155
        Width = 55
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*Weight'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 10
      end
      object txtDesc: TStaticText
        Left = 5
        Top = 85
        Width = 38
        Height = 17
        Caption = '*Desc'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 11
      end
      object edDesc: TEdit
        Left = 50
        Top = 82
        Width = 275
        Height = 21
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 30
        ParentFont = False
        TabOrder = 12
        Text = 'edDesc'
      end
      object edCode: TEdit
        Left = 50
        Top = 57
        Width = 148
        Height = 21
        AutoSize = False
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 20
        ParentFont = False
        TabOrder = 13
        Text = 'edCode'
      end
      object txtCode: TStaticText
        Left = 5
        Top = 60
        Width = 38
        Height = 17
        Caption = '*Code'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 14
      end
      object txtUDC: TStaticText
        Left = 5
        Top = 35
        Width = 35
        Height = 17
        Caption = '*UDC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 15
      end
      object edUDC: TEdit
        Left = 50
        Top = 32
        Width = 148
        Height = 21
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 20
        ParentFont = False
        TabOrder = 16
        Text = 'edUDC'
      end
      object txtLocation: TStaticText
        Left = 195
        Top = 10
        Width = 58
        Height = 17
        Caption = '*Location'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 17
      end
      object edLocation: TMaskEdit
        Tag = 1
        Left = 259
        Top = 7
        Width = 65
        Height = 21
        AutoSize = False
        Color = clMoneyGreen
        EditMask = '!999999999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 9
        ParentFont = False
        ReadOnly = True
        TabOrder = 18
        Text = '000000000'
        OnClick = AzzeraTimerClick
      end
      object edSource: TMaskEdit
        Tag = 1
        Left = 5
        Top = 170
        Width = 60
        Height = 21
        AutoSize = False
        Color = clWhite
        EditMask = '!999999999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 9
        ParentFont = False
        TabOrder = 19
        Text = '000000000'
        OnClick = AzzeraTimerClick
      end
      object edDest: TMaskEdit
        Tag = 1
        Left = 75
        Top = 170
        Width = 60
        Height = 21
        AutoSize = False
        Color = clWhite
        EditMask = '!999999999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 9
        ParentFont = False
        TabOrder = 20
        Text = '000000000'
        OnClick = AzzeraTimerClick
      end
      object edQty: TMaskEdit
        Tag = 1
        Left = 150
        Top = 170
        Width = 55
        Height = 21
        AutoSize = False
        Color = clWhite
        EditMask = '!99999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 5
        ParentFont = False
        TabOrder = 21
        Text = '00000'
        OnClick = AzzeraTimerClick
      end
      object edHeight: TMaskEdit
        Tag = 1
        Left = 210
        Top = 170
        Width = 55
        Height = 21
        AutoSize = False
        Color = clWhite
        EditMask = '!99999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 5
        ParentFont = False
        TabOrder = 22
        Text = '00000'
        OnClick = AzzeraTimerClick
      end
      object edWeight: TMaskEdit
        Tag = 1
        Left = 270
        Top = 170
        Width = 55
        Height = 21
        AutoSize = False
        Color = clWhite
        EditMask = '!99999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 5
        ParentFont = False
        TabOrder = 23
        Text = '00000'
        OnClick = AzzeraTimerClick
      end
      object EdData: TMaskEdit
        Tag = 6
        Left = 259
        Top = 32
        Width = 65
        Height = 21
        Color = clCream
        EditMask = '!99/99/9999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 10
        ParentFont = False
        TabOrder = 24
        Text = '01/01/2000'
      end
      object EdOra: TMaskEdit
        Tag = 7
        Left = 259
        Top = 57
        Width = 65
        Height = 21
        Color = clCream
        EditMask = '!99.99.99;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 8
        ParentFont = False
        TabOrder = 25
        Text = '  .  .  '
      end
      object txtDate: TStaticText
        Left = 215
        Top = 35
        Width = 36
        Height = 17
        Caption = '*Date'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 26
      end
      object txtTime: TStaticText
        Left = 215
        Top = 60
        Width = 36
        Height = 17
        Caption = '*Time'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 27
      end
      object txtShift: TStaticText
        Left = 245
        Top = 110
        Width = 70
        Height = 17
        Alignment = taCenter
        AutoSize = False
        Caption = '*Shift'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clPurple
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 28
      end
      object edShift: TEdit
        Left = 245
        Top = 127
        Width = 70
        Height = 21
        AutoSize = False
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 4
        ParentFont = False
        TabOrder = 29
        Text = 'edShift'
      end
    end
  end
  object PnBtn: TPanel
    Left = 0
    Top = 563
    Width = 362
    Height = 44
    Align = alBottom
    BevelOuter = bvLowered
    TabOrder = 4
    object BtnAdd: TSpeedButton
      Left = 5
      Top = 3
      Width = 38
      Height = 38
      Glyph.Data = {
        F6060000424DF606000000000000360000002800000018000000180000000100
        180000000000C006000000000000000000000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8CAC8BC5E10
        BC5B0EBB590DB9570CB7570CB6570FC7C9C8C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8C5C1C06417DF9544E08927E48C1EEB9018CA6708C6C3C1C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8C5C1C06114DD903DDC821FE08417E7880FC76304C6
        C3C1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8C5C1BE6113DC8C39DB801BDF81
        13E6850BC76101C6C3C1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8C5C1BE5F12
        DB8B37DA7E17DF7F0FE78306C56000C6C3C0C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C7C4C1BD5E10DB8B34DA7C15DE7D0CE58103C45E00C6C2C0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C7C4C1BD5E10DA8833D97B13DE7C0AE68001C55F00C6
        C2C0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4CAC7C3
        C9C3BAC9C2BAC9C2BAC9C2BAC8C1BAC8C1BAC7C4C2BD5D10DA8832D97A12DE7B
        09E37E01C35D00C5C3C2C5BEBAC5BFBAC4BEBAC4BEBAC4BEBAC3BDBAC6C3C3C8
        D0D4C8D0D4BA5B0DBC6C39BB6831BB662BBB6427BB6321BB611CBE6118C26718
        D47A1BD97A12DD7B09E37D01D16B00CA6400CF6900D26C00D26C00D26C00D26C
        00D56F00A34008C8D0D4C8D0D4C26513CB977FC68660C68255C7804CC97E41CB
        7C38CF7B2FD17A25D57A1CD97A13DD7B0AE27D02E98300EF8900F69000FE9800
        FE9B00FE9C00FE9C00FEA300A44106C8D0D4C8D0D4C16312CC977CC6865EC682
        54C7804BC97D41CB7C38CE7B2FD17B27D57B1ED97B15DD7C0CE27E03E88200EE
        8800F48E00FA9400FE9800FE9900FE9900FEA000A23F06C8D0D4C8D0D4C06112
        CC9880C6865FC78356C8814DCA7F43CC7D3ACE7C32D17C29D57C20D97D18DE7E
        0FE27F07E88200EE8800F48E00FA9400FE9800FE9900FE9900FEA000A54106C8
        D0D4C8D0D4C06213CD9B83CC9573CC926ACD8F62CF8D5AD18C51D58C49D68C42
        D98B39D97E1BDE8013E3810BE98403F19519F79B19FDA119FEA519FEA619FEA5
        19FEAB19A54306C8D0D4C8D0D4BF6011B5612EB55F27B55E24B55C21B65C1EB6
        5B1AB65A16B55712DA8C3CDA811FDE8218E48510D06C06C96401CD6700CF6900
        D36D00D26C00D26C00D67000A54309C8D0D4C8D0D4C9C6C2C8C1B9C8C1B9C8C0
        B9C7C0B9C7C0B9C6BFB9C5C2C0B85A14DC8F42DA8324DF841DE58816C15E07C5
        C3C3C4BEBCC3BEBCC3BEBCC3BDBCC3BDBCC3BEBCC5C5C4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C6BFB9B85A16DC9246DB8629DF87
        22E68C1DC2610AC5C2C1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C6BFB9B85C18
        DD954BDC892FE08A29E88F23C2610DC5C2C1C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C5BEB9B85E1BDE9750DD8C35E18E2FE7932AC46412C5C2C1C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C5BEB9B9601EDF9B55DE903CE29136E89631C46714C5
        C2C1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C6BEB9B96121DF9E5DDF9443E295
        3DEA9A38C46818C5C2C1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C5BEB9B65A1B
        E5A868E8A861EAA95BEEA546C76D1EC5C2C1C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C7C3C1AD4C0FAA4C0FA94B0EA84A0DA7480EA64710C6C8C8C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4}
    end
    object BtnDel: TSpeedButton
      Left = 42
      Top = 3
      Width = 38
      Height = 38
      Glyph.Data = {
        06030000424D06030000000000003600000028000000180000000A0000000100
        180000000000D002000000000000000000000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4CA813AC66912C36512C16212BE5F12BD5D12B85C12B65712B55612
        B25512AE5112AB4F12A94C12A64A12A44712A044129D42129B3F12993D12973B
        12983B12A55F3DC8D0D4C8D0D4C76A15C9947ACD9574D09974D1976CD39460D4
        9156D78F49D98D3FDC8C34E08B29E58B1EEA8C15EF8C0AF58F00FC9600FE9D00
        FE9F00FE9F00FE9F00FEA500A54306C8D0D4C8D0D4C76A11C99175CC926ECF96
        6DD19366D1905BD38D50D48A46D7893BDA8832DD8728E1871EE58714EA880AEF
        8A01F58F00FC9600FE9900FE9A00FE9900FEA000A34006C8D0D4C8D0D4C66811
        C89174CC916DCF966CD19566D1905CD28D51D48B47D6893DD98833DD8729E087
        1FE48716E9880CEE8903F38D00F99300FE9900FE9900FE9900FEA000A23F06C8
        D0D4C8D0D4C56711C89074CB916DD0946ED29469D1915DD28E53D48B49D6893F
        D88835DC872BDF8722E38718E7880FEC8806F18B00F69000FC9600FE9900FE9A
        00FEA000A54106C8D0D4C8D0D4C46611C79074CB906DCE956DD09669D1915FD2
        8E55D38C4BD58A41D88837DB872EDE8725E2871CE68713EA880AEE8902F38D00
        F79100FB9500FE9900FE9F00A54306C8D0D4C8D0D4C36614C8937DCB9576CE98
        76D39B74D3976BD5945FD69155D88F49DA8D3FDE8E37E18C2CE58D23E98D19EE
        8F10F18E08F69101FA9400FE9900FE9A00FEA300A54306C8D0D4C8D0D4C37A39
        BE600EBC5C0EBA5B0EB85911B55712B45512B25312AF5212AB4E12AA4C12A84E
        16A54B16A24917A046169E44169B41169C40179B441B9B431B9C441BA86645C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4}
    end
    object BtnFw: TSpeedButton
      Left = 85
      Top = 3
      Width = 38
      Height = 38
      Glyph.Data = {
        0A070000424D0A07000000000000360000002800000019000000170000000100
        180000000000D406000000000000000000000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C9C4BEC97A3CCCB9A7C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C9C0B8C88249CAC6BEC8D0D4C8D0D4C8D0D4C8D0D400C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C9C4C0B55F22D06A00BE
        5B0CCEB7A1C8D0D4C8D0D4C8D0D4C8D0D4C8C0B9B75E1BCC6600BF671ECCC4BA
        C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        C5C2B25D24D16B00FE9F00F08A00C36109D1B69BC8D0D4C8D0D4C7C1BBB2591C
        D77100FEA000E27C00C2691ACAC1B5C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C7C5C3B05B25C96300F69000F79100FE9900E57E00D67112
        C7CFD3C7C1BDAE551ECE6800F89200F89200FE9C00DC7600C87A2EC8D0D4C8D0
        D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C7C5C4AC5927C15B00EC8600EA8400
        F18B00E98300C15F08C9B49EC5C1BEAB5420C66000EC8600EB8500F28C00E781
        00C4630CCBBAA7C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C6C5C6A75729
        BA5400E27C00E17B00E78100DF7900BD5B08C8B29CC2C0C0A54F20BE5800E27C
        00E27C00E88200DE7800BF5D0CCDB9A5C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4
        C8D0D4C5C6C7A7552BB24C00D67000D56F00DC7600D67000B85607C7AF9AC3C1
        C2A24E23B65000D87200D67000DD7700D56F00BB580CCDB8A4C8D0D4C8D0D4C8
        D0D4C8D0D400C8D0D4C8D0D4C6C8C8A2532DAB4500CD6700CC6600D06A00CD67
        00B25107C4AD99C3C3C39E4C25AE4800CE6800CC6600D16B00CD6700B5540BCC
        B6A3C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C6C7C8A4542FA74204C35C
        00C15B00C55F00C45E00AF4E07C3AB96C4C4C3A04C27AA4503C45D00C25C00C6
        6000C35D00B14F0ACAB3A0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0
        D4A35934AC511CC56E25BB5A0ABA5400B95300AA4302C1A895C7CBCEA2522AAF
        541CC46A20BA5805BB5500BA5300AA4808C6B0A0C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400C8D0D4AB7355A54716C77B44BC6526B7570DB65000AC
        4701BD8C67C6CCCFA45733AF5824C5793FBB6220B75508B44F00AC4804C4A58D
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C7CED2AD7051A5
        4717C27947BA6429B75812B85200B14C01C0926BC5C6C8A15330AC5526C07542
        B96224B7560CB75000B34D05CBAB91C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D400C8D0D4C8D0D4C7CED2AA6E4DA54917C2794CBC682EBB5E15BD5701B55101
        C3976FC5C7C8A1512EAD5828C17647BC6527BB5B0FBB5500B65306CBAD95C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8CED2AC6F4AAB5019
        C6814FC26F31C26618C35E03BA5601C69A74C5C5C6A5512BB25D2BC57D4AC16C
        2CC36414C25C00BB5807CEB198C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4
        C8D0D4C8D0D4C7CDD1B06E47B0541ACE8954C97935CA701CCB6705C05A01C8A0
        79C5C5C5A55429B8652DCC864EC9762ECB6D17C96402BF5D07CFB59BC8D0D4C8
        D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C7CDD0B06E44B65C1CD692
        57D38438D47B1FD57107C55F02CAA47CC4C3C3AA5727BE6C30D48F51D17F31D4
        781AD16C04C56309CCB69FC8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8CCCFB47042BC621EDE9B5BDB8D3CDF8826D57308CA690DCACECEC6
        C3C2B05925C47332DD9954DA8A37DF8620D06E05C87220C8D0D4C8D0D400C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C7CBCEBA723FC16821E7A861D6
        832DC46511CDBCA8C8D0D4C8D0D4C7C3C0B25C23CB7B37E7A65CD37E26C36716
        CBBEAFC8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8CBCDBC743DC26518BF6518CCBFB2C8D0D4C8D0D4C8D0D4C8D0D4C9C4BF
        B76020C86F23BF661ECBC3B9C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8CBCDC99163CAC4BBC8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C9C3BECB8852C9C5C0C8D0D4C8D0D4C8D0D4C8D0
        D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D400}
    end
    object BtnRw: TSpeedButton
      Left = 122
      Top = 3
      Width = 38
      Height = 38
      Glyph.Data = {
        0A070000424D0A07000000000000360000002800000019000000170000000100
        180000000000D406000000000000000000000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4CBB8A5C97B
        3EC8C5C2C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4CBBCAFC9712CCBB8AAC8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0
        D4C8D0D4CFB59EBF5E0BD07F30B76228C8C7C5C8D0D4C8D0D4C8D0D4C8D0D4CD
        BBA8C05F10D88B3FB55411C7BBAFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400C8D0D4CBB195C46107EBAD64FCC474CF7B2BB4622DC7
        CAC8C8D0D4C8D0D4D0B9A0C5630CE6A65CFAC374DF9544B15015C8BCB4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C76811E09B4FF3
        BB72F1B25EF7B659C96E1EB16332C7C9CBC8D0D4D57E2FD98B3BF5BD76F1B25F
        F6B357DA872FAB4E18C7BEB9C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D400C8D0D4CCB69FC55F08E5A256F0AB56EFA548F7AB40C06211AF6439C7CBCD
        C9C4BAC2661ADD9245EFAE5BEDA548F5A840D4791EA84F1CC7C0BDC8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4CFB398C05B06E39946EDA03F
        ED9A30F59E28B95609AB643EC6CCCFC8C0B5BD5F15DB8C39EDA141EB9A32F39D
        28CB6C0EA44F21C6C3C1C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4
        C8D0D4CDAD91BA5504E29034E99428EC8F1BF3930FB24D01AA6845C7CDD1C7BE
        B0B95811D8842BEA952BEA8F1CF29311C35F02A14F25C6C4C4C8D0D4C8D0D4C8
        D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4CCA788B85204E18720E78A12E983
        05F08A00AB4600A96B4CC7CED2C7B8AAB7540DD77D1CE78916E88306F18C00BD
        57009F502AC6C7C8C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4CBA080B24E04DF7E0DE57E01E88200EF8900A64100B07353C7CFD3CAB4A4B1
        4E0BD5750EE58001E88100F28C00B65000A15330C7C8CAC8D0D4C8D0D400C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C29677AA4602DD7700E27C00E88200EE
        8800A84200BA7E62C8D0D4C4AE9DA94607D46E00E47E00E68000F18B00B75100
        A45C39C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C39575AA
        4601D97300DE7800E47E00E98300A64001BC8468C8D0D4C39F84AB4402D46E00
        DE7800E37D00EA8400A94400AE7355C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4CB9D7DB44B03D3710BD77200DC7600E27C00A43D00B7785BC8D0D4
        CAA58BB24C04D06F0CD97101DA7400E17B00A84200AC6C4CC7CED2C8D0D4C8D0
        D400C8D0D4C8D0D4C8D0D4C8D0D4CDA483B75003CF751ED27410D46E03D97300
        A64000B07353C7CFD3CBAC91B65004CD741DD27512D26E05DA7400AB4400A966
        45C7CDD1C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4CDA989BB5604CD7B31
        D07A27CF7318D5720CAA4500AD6F4CC7CED2CCB097BB5706CB782FD07A29CF74
        1AD4740EAF4901A8623EC6CCCFC8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4
        CFAE90BF5B04CD8243CE813CCD792DD17A23B04B07AE6D46C7CDD1C9B49CBF5D
        07CC7F40CE8340CD7A30D17A26B45109AA6239C8CACDC8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D400C8D0D4CDB196C76106CE8A56CE8A53CE8243D0823CB5540EB16B
        3FC7CDD0CBB9A2C5650ACE8953CF8C57CD8347CF823FB85A13AE6333C7C9CBC8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C76F1BCF894FD0976FCD8D
        5BCF8B55B95D19B46839C8CBCDC8D0D4D5883DCD7E3BD09974CE8E5DD08C57BC
        6320B0622EC7C8C8C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0
        D4CBBBAAC66610CF8F5FD19972BE6725B86834C7CACBC8D0D4C8D0D4CBC7C2C6
        7125CD8348D19B75C16D2EB36229C8C7C6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4CBBCACC06111C26D2AB9672EC7CAC8C8
        D0D4C8D0D4C8D0D4C8D0D4C9C8C3C16C27C26C25B96124C8C6C3C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D400C8D0D4C8D0D4C8D0D4CC
        BDADCA8147C9C8C6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4CAC7C3C98953
        C9C4BFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D400C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D400}
    end
    object BtnOK: TBitBtn
      Left = 170
      Top = 3
      Width = 90
      Height = 38
      Caption = '*OK'
      Default = True
      ModalResult = 1
      TabOrder = 0
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
    object BtnEsci: TBitBtn
      Left = 265
      Top = 3
      Width = 90
      Height = 38
      Caption = '*Esci'
      ModalResult = 3
      TabOrder = 1
      Glyph.Data = {
        060E0000424D060E000000000000360000002800000022000000220000000100
        180000000000D00D000000000000000000000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        0000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C7CFD3C1C9CDB4BBBEB0B8
        BBB8C0C3C3CACEC7CFD3C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C3CBCF
        A7AEB1777C7E6A6E708186889EA4A7B2B9BDBFC6CAC6CED2C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C7CFD3C7CFD3C7
        CFD3C7CFD3B8B8BB8964657E56576C5D5E535354585B5D73777991979AAAB1B4
        BAC1C5C3CACEC7CFD3C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C6CED2BBC3
        C7ADB4B8A9B0B4A9B0B4AAB1B49D8F92C19999B284848957577E4D4D765B5C5E
        585A515456666A6C84898CA1A7AAB4BBBEBFC7CBC6CED2C8D0D4C8D0D4C8D0D4
        0000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4BCC4C892989B64686A575B5C575B5C595D5F776C6DCDA5A5B587879F6C
        6C9E6B6B8F5B5B804D4D7B5454675B5C5051535B5F60767B7E959B9EAEB5B9BF
        C7CBC7CFD3C8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D49DA2A5696969666666656565666666636565615F5F
        D1A8A8B78989A06D6D9F6C6CA06D6DA26F6F95626284515176464671595A5A56
        585256576E72749BA1A4BFC6CAC8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4787A7B8282826D6C6C777778AC
        A7AAAEABAEA1A0A1D0A6A7B88A8AA06D6DA16E6EA16E6EA26F6FA16E6EA37070
        9E6B6B8956567945456E51525154566C7173B1B8BCC8D0D40000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C7CFD37172729696
        966F6D6E5F7D6863B0797DB78E8DB498D3A7A9BB8C8DA26F6FA37070A37070A2
        6F6FA26F6FA26F6FA26F6FA37070A47171845050454A4A5D6163ACB3B6C8D0D4
        0000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C6CED2C1C9CD
        C3CACE717272989898706D704D825B1BB0421BAC411EA440DAAAAEBC908EA572
        72A57272A47171A47171A47171A37070A37070A26F6FA571718B585845494A5D
        6163ACB3B6C8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4BFC6CAA6ADB0A8AFB27576769D9D9D716E714C7E5619A2381C9F391E9A39
        DCADB1BE9090A57272A67373A67373A67373A57272A57272A47171A47171A571
        718D5A5A494D4E5D6163ACB3B6C8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D477AE7E819188757A7C7576769F9F9F7471734A785313
        972D15952E1B8E2EDDADB2BF9593A77474A87575A87575A77474A77474A77474
        A67373A57272A874748D5B5B4A4E4E5E6264ACB3B6C8D0D40000C8D0D4C8D0D4
        C7CFD3C7CFD3C7CFD3C7CFD3C7CFD3C7CFD35EA76424922C545F598080819F9F
        9F75727447744D0D891F0F8822188225E0B1B5C19697A97676AA7777AA7777A4
        7272A57373A97676A87575A77474A874748F5D5D4C50505E6264ACB3B6C8D0D4
        0000C8D0D4C4CCD0B8BFC3B3BABEB2B9BDB2B9BDB2B9BDB1B8BC599C5F3EC26E
        218B299DAE9DAAAAAA767376437149067D13097C1613781AE3B4B9C39897AC78
        78AC7878A57272A07070A37272A77575A97676A97676AB7777915F5F4E53535E
        6264ACB3B6C8D0D40000C8D0D4B6BDC1858B8D7175786F74766F74766F73756E
        72744B81503DBF6D3EBE6C23992AB7C8B78583853F6A40006D00006D020E690B
        E4B5B9C59B9BAD7878A78888858585887B7BA27070A67474AA7777AA7777AB77
        779361614F54545F6365ADB3B7C8D0D40000C8D0D4418F4536843D337E39337F
        39348138338537328439187F1A47E67F45D9783FBD6E27A02CA1B2A064887132
        9F59339D593D945AE4B5B8C69B9BB48A8AB9BEBEC3C4C4ABAFAFA27575AD7979
        AC7979AC7979AD7A7A9663635255555F6365ADB3B7C8D0D40000C8D0D4208F2F
        3CC26F38BD6839BE6A3BBD6B3CBD6C3EBE6D3DBA6B41CB714BE87F49DA7A42BC
        6F249E29A7CCBAB7FEFEAEFEFEAEFEFEE1B5B5C89D9DBD9898ECEEEEF8F9F9C8
        CBCBAF7C7CB07C7CAE7B7BAD7A7AAD7A7A9766665458585F6365ADB3B7C8D0D4
        0000C8D0D41B8F2C4EE1833CD8703FDB7343DF7744E07848E47C4AE67E4BE67F
        4CE6804EEA834DDD7F45BC7125A62FC9F7E3CBFEFEC6FEFEE1B5B5CCA4A4B17B
        7BCFBDBDC6C2C2B69F9FB17C7CB07D7DB07D7DAF7C7CB07D7D996767565B5C60
        6465ADB3B7C8D0D40000C8D0D4198F2A5EE38E3CD77040D97343DC7646DF7948
        E17B4AE37D4CE57F4FE88250E98354EF8851E18247BD732CAE33D5F4E3E3FEFE
        E1B6B6CDA5A5B37F7FB37E7EB47E7EB37F7FB38080B27F7FB17E7EB07D7DB07D
        7D9B6969595D5D606465ADB3B7C8D0D40000C8D0D4188F296BE9983BD86F42DB
        7545DE7848E17B4AE37D4DE6804FE88251EA8453EC8655EE8858F48C56E4884D
        C1784DB84DFEFEFEE4B9BACFA7A7B58282B68383B58282B58282B48181B38080
        B38080B27F7FB380809D6B6B5A5E5E606465ADB3B7C8D0D40000C8D0D4159127
        7BEBA339DA6E41DD7544E07847E37C49E57D4CE8814EEA8354ED8756EF8958F1
        8B5EF9935CED9231AF35E6F4E3FEFDF9E5BCBCD1ABABB78484B88585B78484B7
        8484B68383B58282B48181B38080B380809E6D6D5C6060606465ADB4B8C8D0D4
        0000C8D0D41692278EF0B25CE08863E38D67E7926AE8946DEC9770ED9A75F19E
        5CF08D59F38C5EF79363FD9A2FB334E6F6DFFEF8EFFEF7EDE6BDBDD1ADADB986
        86B98686B98686B88585B88585B78484B68383B48181B48181A06F6F5D616160
        6465ADB4B8C8D0D40000C8D0D41D983072F5A674F0A278F3A67CF4A97EF8AB83
        F9B182FBAD9FF8BE5AF28D5EF79264FE9B2AAC30BECBB9FEF9EAFEF4E4FEF4E4
        E9C1C1D3ADADBC8888BA8787BB8787BA8787B98686B88585B78484B68383B784
        84A171715E6262606466ADB4B8C8D0D40000C8D0D4499E4D3F9F4A3EA1453CA1
        453EA5463DA7453FA7451D8E1F7FF7A85BF89164FE9B2EAF35A7B7A8B5ADA5FE
        F1DFFEEEDBFEEEDBEAC1C2D6B0B0BA8686D9A9A9D3A2A2C59191BD8888B78282
        B58282B68383B78484A372725F6263616567ADB4B8C8D0D40000C8D0D4C8D0D4
        C7CFD3C7CFD3C7CFD3C7CFD3C7CFD3C7CFD35EA56476FDA763FE9A2CAB33C5D6
        C58D8E8EB2A79CFEECD5FEE8D2FEE8D2ECC4C4D6B1B1BB8686F4DCDCFEECECFE
        E7E7FDDCDCF7CECEE9BBBBDCABABBC8989A474745F6465616567ADB4B8C8D0D4
        0000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D45FAA6569FEA1
        28A131A9BCAAC4C4C4848586B1A498FEE8CCFEE5C9FEE5C9EEC5C6D6B3B3BD87
        87F0DADAFEEDEDFEE8E8FEE4E4FEE1E1FEDDDDFEDADAC18D8DA7767661656661
        6567ADB4B8C8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D460B1652CA8389BB0A5949595C1C1C1868789B1A292FEE3C3FEDFC0FEDFC0
        F1C8C9D8B6B6C08B8BEEDCDCFEF8F8FEF1F1FEE9E9FEE3E3FEDEDEFED7D7BC88
        88A77878626566616567ADB4B8C8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D47EBC86AEC4B9C3CACE8E8F8FC1C1C188898BB3A28EFE
        DDB8FEDAB6FEDAB6F1C9CAD9B6B6C38E8ECAA3A3DDC0C0E4CACAEFD3D3FADEDE
        FEE4E4FEDBDBBB8686AA7979636667616567ADB4B8C8D0D40000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C7CFD3909191C3C3
        C38B8C8EB8A18BFEDFB1FEDCAFFEDCAFF4CCCDDBB7B7C59191C59191C38F8FC0
        8C8CBC8787B98585B78383BE8C8CBA8787AB7A7A656869636769ADB4B8C8D0D4
        0000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C7CFD3919292C9C9C98F9091837D798D837893887BA38C82FCD9D9E2C8C8CD9F
        9FC79494C59191C38F8FC18D8DBF8B8BBE8B8BBC8989BA8787AD7C7C64696964
        686AAEB5B9C8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4939494CACACA8F8F8F9292938F90918C8D8E908B8B
        BC9999C39F9FCCA7A7D4AFAFD8AFAFD8ABABD6A7A7D4A3A3D0A0A0CE9C9CCC97
        97B07E7E666B6B6D7174B1B8BCC8D0D40000C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4969797D6D6D6C0C0C0C0C0C0BE
        BEBEB7B7B7B4B4B4ACB0B0A6A9A9A4A7A79FA1A1999B9B969797989494989191
        968C8C998A8A948383837B7B6B6E6F92989BBCC4C8C8D0D40000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4AEB2B48E91
        918D8F8F8C8C8D8C8C8D8B8C8D8B8C8D86888886888885878784858582838480
        81828081827E80807E80807B7D7D7D7F7F7A7C7D9EA3A6BCC4C8C6CED2C8D0D4
        0000C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D40000}
    end
  end
  object TimerClose: TTimer
    Enabled = False
    Interval = 90000
    OnTimer = TimerCloseTimer
    Left = 305
    Top = 205
  end
end
