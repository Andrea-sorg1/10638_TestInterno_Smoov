object FormBaie: TFormBaie
  Left = 682
  Top = 201
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = '*Editor BAIE'
  ClientHeight = 459
  ClientWidth = 557
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
  ShowHint = True
  OnActivate = FormActivate
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object PanBaia: TPanel
    Left = 0
    Top = 0
    Width = 557
    Height = 459
    Align = alClient
    BevelInner = bvLowered
    TabOrder = 0
    OnMouseMove = AzzeraTimerMouseMove
    object lblPriorita: TLabel
      Left = 339
      Top = 40
      Width = 102
      Height = 13
      Caption = '*Priorita calcolata'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object EditPanel: TPanel
      Left = 440
      Top = 5
      Width = 106
      Height = 26
      BevelInner = bvLowered
      Caption = '*EDITAZIONE'
      TabOrder = 5
    end
    object NRecordPanel: TPanel
      Left = 325
      Top = 5
      Width = 106
      Height = 26
      BevelInner = bvLowered
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 6
    end
    object TXNum: TStaticText
      Tag = 1
      Left = 10
      Top = 14
      Width = 52
      Height = 17
      Caption = '*Numero'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 7
    end
    object EdNum: TMaskEdit
      Tag = 1
      Left = 85
      Top = 10
      Width = 33
      Height = 21
      Color = clCream
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
      OnClick = AzzeraTimerClick
    end
    object GBTipo: TRadioGroup
      Left = 220
      Top = 110
      Width = 106
      Height = 111
      Caption = '*Tipo baia:'
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      Items.Strings = (
        '*None'
        '*PRELIEVO'
        '*DEPOSITO')
      ParentColor = False
      ParentFont = False
      TabOrder = 4
      OnClick = AzzeraTimerClick
    end
    object EdSig: TEdit
      Left = 130
      Top = 10
      Width = 121
      Height = 21
      CharCase = ecUpperCase
      Color = clCream
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 11
      ParentFont = False
      TabOrder = 1
      Text = 'EDIT1'
      OnClick = AzzeraTimerClick
    end
    object TXUrg: TStaticText
      Tag = 1
      Left = 10
      Top = 41
      Width = 48
      Height = 17
      Caption = '*Urgenza'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 8
    end
    object EdUrg: TMaskEdit
      Tag = 5
      Left = 85
      Top = 37
      Width = 15
      Height = 21
      Color = clCream
      EditMask = '!9;1; '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 1
      ParentFont = False
      TabOrder = 2
      Text = '9'
      OnClick = AzzeraTimerClick
      OnDblClick = AzzeraTimerClick
    end
    object TextPriorita: TStaticText
      Left = 445
      Top = 35
      Width = 96
      Height = 21
      Alignment = taCenter
      AutoSize = False
      BevelKind = bkSoft
      Caption = '99'
      Color = clSkyBlue
      Font.Charset = ANSI_CHARSET
      Font.Color = clGray
      Font.Height = -12
      Font.Name = 'Verdana'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      TabOrder = 9
    end
    object TXNodo: TStaticText
      Tag = 1
      Left = 10
      Top = 69
      Width = 34
      Height = 17
      Caption = '*Nodo'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 10
    end
    object EdNod: TMaskEdit
      Tag = 1
      Left = 85
      Top = 65
      Width = 33
      Height = 21
      Color = clCream
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
      OnClick = AzzeraTimerClick
    end
    object GBPlc: TGroupBox
      Left = 335
      Top = 65
      Width = 217
      Height = 186
      Caption = '*Consensi PLC:'
      TabOrder = 11
      object Bevel5: TBevel
        Left = 15
        Top = 95
        Width = 186
        Height = 11
        Shape = bsTopLine
      end
      object CBBlocco: TCheckBox
        Left = 115
        Top = 47
        Width = 86
        Height = 17
        Caption = '*Blocco'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnClick = AzzeraTimerClick
      end
      object CBTuttoPieno: TCheckBox
        Left = 115
        Top = 72
        Width = 86
        Height = 17
        Caption = '*Tutto pieno'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        OnClick = AzzeraTimerClick
      end
      object CBFineOp: TCheckBox
        Left = 10
        Top = 145
        Width = 100
        Height = 17
        Caption = '*Fine operazione'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        OnClick = AzzeraTimerClick
      end
      object CBTgvIng: TCheckBox
        Left = 10
        Top = 125
        Width = 100
        Height = 17
        Caption = '*Tgv in ingombro'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        OnClick = AzzeraTimerClick
      end
      object CBOkIng: TCheckBox
        Left = 115
        Top = 105
        Width = 100
        Height = 17
        Caption = '*Ok ingresso'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        OnClick = AzzeraTimerClick
      end
      object CBRicIng: TCheckBox
        Left = 10
        Top = 105
        Width = 100
        Height = 17
        Caption = '*Rich. ingresso'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 5
        OnClick = AzzeraTimerClick
      end
      object EdPlc: TMaskEdit
        Tag = 1
        Left = 73
        Top = 20
        Width = 35
        Height = 21
        Color = clCream
        EditMask = '!99;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 2
        ParentFont = False
        TabOrder = 6
        Text = '00'
        OnClick = AzzeraTimerClick
      end
      object lblIDPLC: TStaticText
        Tag = 1
        Left = 10
        Top = 24
        Width = 41
        Height = 17
        Caption = 'Id. Plc'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 7
      end
      object CBOkForcAlte: TCheckBox
        Left = 115
        Top = 125
        Width = 100
        Height = 17
        Caption = '*Ok forche alte'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 8
        OnClick = AzzeraTimerClick
      end
      object CBChiamata: TCheckBox
        Left = 115
        Top = 145
        Width = 100
        Height = 17
        Caption = '*Chiamata'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 9
        OnClick = AzzeraTimerClick
      end
      object CBAutomatico: TCheckBox
        Left = 115
        Top = 22
        Width = 86
        Height = 17
        Caption = '*Automatico'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 10
        OnClick = AzzeraTimerClick
      end
      object lblSollev: TStaticText
        Tag = 1
        Left = 6
        Top = 74
        Width = 40
        Height = 17
        Caption = '*Sollev.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 11
      end
      object EdSollev: TMaskEdit
        Tag = 1
        Left = 73
        Top = 70
        Width = 35
        Height = 21
        Color = clCream
        EditMask = '!9999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 4
        ParentFont = False
        TabOrder = 12
        Text = '0000'
        OnClick = AzzeraTimerClick
      end
      object EdAltez: TMaskEdit
        Tag = 1
        Left = 73
        Top = 45
        Width = 35
        Height = 21
        Color = clCream
        EditMask = '!9999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 4
        ParentFont = False
        TabOrder = 13
        Text = '0000'
        OnClick = AzzeraTimerClick
      end
      object TXAltez: TStaticText
        Tag = 1
        Left = 5
        Top = 49
        Width = 65
        Height = 17
        Caption = '*Altezza baia'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 14
      end
    end
    object RGDati: TGroupBox
      Left = 10
      Top = 225
      Width = 316
      Height = 231
      Caption = '*dati postazioni:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 12
      object PCDatiPal: TPageControl
        Left = 10
        Top = 15
        Width = 296
        Height = 211
        ActivePage = TSPos1
        TabIndex = 0
        TabOrder = 0
        OnChange = PCDatiPalChange
        object TSPos1: TTabSheet
          Caption = '01'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          object lblPresCestA: TLabel
            Left = 252
            Top = -4
            Width = 12
            Height = 13
            Alignment = taCenter
            Caption = '000'
            Font.Charset = ANSI_CHARSET
            Font.Color = clGray
            Font.Height = -8
            Font.Name = 'Arial Narrow'
            Font.Style = []
            ParentFont = False
          end
          object lblPresCestB: TLabel
            Left = 252
            Top = 20
            Width = 12
            Height = 13
            Alignment = taCenter
            Caption = '000'
            Font.Charset = ANSI_CHARSET
            Font.Color = clGray
            Font.Height = -8
            Font.Name = 'Arial Narrow'
            Font.Style = []
            ParentFont = False
          end
        end
        object TSPos2: TTabSheet
          Caption = '02'
          ImageIndex = 1
        end
        object TSPos3: TTabSheet
          Caption = '03'
          ImageIndex = 2
        end
        object TSPos4: TTabSheet
          Caption = '04'
          ImageIndex = 3
        end
        object TSPos5: TTabSheet
          Caption = '05'
          ImageIndex = 4
        end
      end
      object CBStato: TComboBox
        Left = 70
        Top = 45
        Width = 106
        Height = 21
        Style = csDropDownList
        Color = clSkyBlue
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 13
        ParentFont = False
        TabOrder = 1
        OnChange = CBStatoChange
        OnClick = AzzeraTimerClick
        Items.Strings = (
          'V - VUOTO'
          'R - RESO'
          'F - FINITO'
          'S - SEMILAVORATO')
      end
      object lblStato: TStaticText
        Tag = 1
        Left = 18
        Top = 50
        Width = 39
        Height = 17
        Caption = '*Stato'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
      end
      object lblPallet: TStaticText
        Tag = 1
        Left = 18
        Top = 74
        Width = 41
        Height = 17
        Caption = '*Pallet'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
      end
      object EdCodPal: TMaskEdit
        Tag = 1
        Left = 70
        Top = 96
        Width = 31
        Height = 21
        Color = clCream
        EditMask = '!99;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 2
        ParentFont = False
        TabOrder = 4
        Text = '00'
        OnClick = AzzeraTimerClick
      end
      object lblCodice: TStaticText
        Tag = 1
        Left = 19
        Top = 99
        Width = 48
        Height = 17
        Caption = '*Codice'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 5
      end
      object CBBasso: TCheckBox
        Left = 15
        Top = 185
        Width = 86
        Height = 17
        Alignment = taLeftJustify
        Caption = '*Basso'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 6
      end
      object CBSovrap: TCheckBox
        Left = 15
        Top = 150
        Width = 86
        Height = 17
        Alignment = taLeftJustify
        Caption = '*Sovrapp'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 7
      end
      object lblAltezza: TStaticText
        Tag = 1
        Left = 18
        Top = 124
        Width = 50
        Height = 17
        Caption = '*Altezza'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 8
      end
      object EdAltPal: TMaskEdit
        Tag = 1
        Left = 70
        Top = 121
        Width = 34
        Height = 21
        Color = clCream
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
        OnClick = AzzeraTimerClick
      end
      object CBForcB: TCheckBox
        Tag = 1
        Left = 266
        Top = 70
        Width = 30
        Height = 12
        Caption = 'B'
        Enabled = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 10
        OnClick = CBForcClick
      end
      object CBForcA: TCheckBox
        Left = 266
        Top = 45
        Width = 30
        Height = 17
        Caption = 'A'
        Enabled = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 11
        OnClick = CBForcClick
      end
      object gbPresa: TGroupBox
        Left = 140
        Top = 120
        Width = 156
        Height = 91
        Caption = '*configurazione presa:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 12
        object CBPos1A: TCheckBox
          Left = 5
          Top = 20
          Width = 46
          Height = 17
          Caption = '1 H'
          Color = clYellow
          ParentColor = False
          TabOrder = 0
        end
        object CBPos2A: TCheckBox
          Left = 55
          Top = 20
          Width = 46
          Height = 17
          Caption = '2 H'
          Color = clYellow
          ParentColor = False
          TabOrder = 1
        end
        object CBPos3A: TCheckBox
          Left = 105
          Top = 20
          Width = 46
          Height = 17
          Caption = '3 H'
          Color = clYellow
          ParentColor = False
          TabOrder = 2
        end
        object CBPos3B: TCheckBox
          Left = 105
          Top = 45
          Width = 46
          Height = 17
          Caption = '3 L'
          Color = clLime
          ParentColor = False
          TabOrder = 3
        end
        object CBPos2B: TCheckBox
          Left = 55
          Top = 45
          Width = 46
          Height = 17
          Caption = '2 L'
          Color = clLime
          ParentColor = False
          TabOrder = 4
        end
        object CBPos1B: TCheckBox
          Left = 5
          Top = 45
          Width = 46
          Height = 17
          Caption = '1 L'
          Color = clLime
          ParentColor = False
          TabOrder = 5
        end
        object TxtStPresenze: TStaticText
          Left = 30
          Top = 70
          Width = 96
          Height = 17
          Alignment = taCenter
          AutoSize = False
          BevelInner = bvLowered
          BevelKind = bkFlat
          BevelOuter = bvRaised
          Caption = 'TxtStPresenze'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clGray
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 6
        end
      end
      object CBCarW: TCheckBox
        Left = 15
        Top = 170
        Width = 86
        Height = 17
        Alignment = taLeftJustify
        Caption = '*CarWash'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 13
      end
      object lblLotto: TStaticText
        Tag = 1
        Left = 104
        Top = 99
        Width = 38
        Height = 17
        Caption = '*Lotto'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 14
      end
      object EdLotPal: TMaskEdit
        Tag = 1
        Left = 145
        Top = 96
        Width = 38
        Height = 21
        Color = clCream
        EditMask = '!99999;1; '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        MaxLength = 5
        ParentFont = False
        TabOrder = 15
        Text = '00000'
        OnClick = AzzeraTimerClick
      end
      object CBRuotato: TCheckBox
        Left = 15
        Top = 202
        Width = 86
        Height = 17
        Alignment = taLeftJustify
        Caption = '*Ruotato'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 16
      end
    end
    object lblGruppo: TStaticText
      Tag = 1
      Left = 130
      Top = 34
      Width = 131
      Height = 17
      Caption = '*Gruppo appartenenza'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 13
    end
    object CBGruppo: TComboBox
      Left = 130
      Top = 50
      Width = 126
      Height = 21
      Style = csDropDownList
      Color = clCream
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 14
      OnClick = AzzeraTimerClick
      Items.Strings = (
        '0 - EMPY'
        '1 - FILLING'
        '2 - DRYNG'
        '3 - END OF DRYING')
    end
    object TxtPuntiAux: TStaticText
      Left = 6
      Top = 92
      Width = 111
      Height = 17
      AutoSize = False
      BevelKind = bkSoft
      TabOrder = 15
      OnClick = TxtPuntiAuxClick
    end
    object CBTipPal: TComboBox
      Left = 200
      Top = 320
      Width = 106
      Height = 21
      Style = csDropDownList
      Color = 10206463
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 16
      OnClick = AzzeraTimerClick
      Items.Strings = (
        'V - VUOTO'
        'R - RESO'
        'F - FINITO'
        'S - SEMILAVORATO')
    end
    object CBGruppoDest: TComboBox
      Left = 130
      Top = 75
      Width = 126
      Height = 21
      Style = csDropDownList
      Color = clCream
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 17
      OnClick = AzzeraTimerClick
      Items.Strings = (
        '0 - EMPY'
        '1 - FILLING'
        '2 - DRYNG'
        '3 - END OF DRYING')
    end
    object BtnBoxA: TBitBtn
      Tag = 1
      Left = 205
      Top = 270
      Width = 65
      Height = 20
      Caption = '*dati '
      TabOrder = 18
      Visible = False
      OnClick = BtnBoxClick
    end
    object BtnBoxB: TBitBtn
      Left = 205
      Top = 292
      Width = 65
      Height = 20
      Caption = '*dati '
      TabOrder = 19
      Visible = False
      OnClick = BtnBoxClick
    end
  end
  object Panel3: TPanel
    Left = 335
    Top = 263
    Width = 217
    Height = 188
    BevelInner = bvLowered
    TabOrder = 1
    OnMouseMove = AzzeraTimerMouseMove
    object BtnIndietro: TBitBtn
      Left = 7
      Top = 5
      Width = 102
      Height = 36
      Caption = '*Indietro'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 7
      OnClick = BtnIndietroClick
      OnMouseMove = AzzeraTimerMouseMove
      Glyph.Data = {
        B60D0000424DB60D000000000000360000002800000030000000180000000100
        180000000000800D0000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C1CDCC83B78BC8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0CBCBCBA3A3A3D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C0CDCB2596316EAD74C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0CACACA6A6A6A949494D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BECCCA27993344E17E6EAC74C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C9C9C9
        6D6D6DB1B1B1949494D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BD
        CCC8279E344AEB863ED0726EAC74C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C9C9C9707070BABABAA3A3A3949494D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4BCCBC729A1354DEA8747E47E3AC16967A76DC8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8C8C8737373BABABA
        B3B3B39797978E8E8ED0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4B9CBC52CA73852
        F08C49E37E48E57D38B6631C922650A66050A55E50A45E4CA35B4BA35A499F57
        4CA15A50A4605C9F62C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0C7C7C7777777C0C0C0B3B3B3B4B4B48E8E8E64646487878786868685
        85858383838383837F7F7F828282868686858585D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4B9CBC42DA93B56F4904DE68248E17B49E37D3CBA673EBF6D3EC0
        6E3DBF6D3CC06C3BC06B3ABF6C3ABE6B39C06A3CC372529B58C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C6C6C6797979C4C4C4B6B6B6B1B1B1
        B2B2B29292929797979898989797979797979797979797979696969696969B9B
        9B7E7E7ED0D0D0D0D0D0C8D0D4C8D0D4C8D0D4B5CAC12FB03F5AF79450E9844D
        E6804CE57F49E37C4AE67E4AE67D47E47B46E37A46E27943DF7742DE7642DE75
        3FDC7441D87A4F9A54C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0C4C4
        C47F7F7FC7C7C7B9B9B9B6B6B6B5B5B5B2B2B2B5B5B5B4B4B4B2B2B2B1B1B1B0
        B0B0AEAEAEADADADACACACABABABAAAAAA7C7C7CD0D0D0D0D0D0C8D0D4C8D0D4
        B4CABE33B74662FB9A52ED8751EA844EE7814DE6804CE57F4AE37D49E27C48E2
        7B46DF7945DE7844DE7742DB7541DA7441DC7546DD7D4C9A52C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0C3C3C3868686CCCCCCBCBCBCBABABAB7B7B7B6B6B6
        B5B5B5B3B3B3B2B2B2B1B1B1AFAFAFAEAEAEADADADABABABAAAAAAABABABAFAF
        AF7B7B7BD0D0D0D0D0D0C8D0D4B1C9BC3CC25481FEB471F49D51ED8552EB8551
        EA844FE8824EE7814DE6804BE47E4AE37D49E27C47E07A46DF7945DE7843DC76
        42DC7649E4834D9B52C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0C2C2C2919191D9D9
        D9CBCBCBBCBCBCBBBBBBBABABAB8B8B8B7B7B7B6B6B6B4B4B4B3B3B3B2B2B2B0
        B0B0AFAFAFAEAEAEACACACABABABB5B5B57C7C7CD0D0D0D0D0D0C8D0D4AEC9B9
        40C95A9DFEC895F8B852EF8753EE8853EC8652EB8551EA844FE8824EE7814CE5
        7F4BE47E4AE37D48E17B47E07A46DF7944DD774CEA874D9D52C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0C1C1C1989898E2E2E2D9D9D9BEBEBEBDBDBDBCBCBCBBBBBB
        BABABAB8B8B8B7B7B7B5B5B5B4B4B4B3B3B3B1B1B1B0B0B0AFAFAFADADADBABA
        BA7D7D7DD0D0D0D0D0D0C8D0D4C8D0D4B0CABA35C14C9AFEC498F9BA53F18954
        EF8954ED8753EC8652EB854EE9824CE88049E67D49E57D48E47C47E37C45E17A
        46E07A4EEA874FA355C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0C2C2C28E8E
        8EE1E1E1DBDBDBBFBFBFBEBEBEBDBDBDBCBCBCBBBBBBB8B8B8B7B7B7B4B4B4B4
        B4B4B3B3B3B2B2B2B0B0B0AFAFAFBABABA828282D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4B3CBBE32C0459AFEC49CFABC56F28B55F08955EE8851ED8665EF9370EE
        9A6BED976AEC9466E99265E89162E88F5BE7894EED894FA556C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0C4C4C48B8B8BE1E1E1DCDCDCC1C1C1BFBFBF
        BEBEBEBCBCBCC3C3C3C6C6C6C4C4C4C2C2C2C0C0C0BFBFBFBEBEBEBABABABCBC
        BC838383D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4B6CCC12EBF409AFEC49E
        FBBF58F38B55F18954F38A98FCBCA0FCC399FCBE96FBBC91F9B88DF8B589F6B2
        89F6B368FAA355AA5DC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0C6C6C6888888E1E1E1DEDEDEC2C2C2C0C0C0C1C1C1DDDDDDE0E0E0DEDEDEDC
        DCDCD9D9D9D7D7D7D5D5D5D5D5D5CFCFCF898989D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4B8CDC32CBC3F98FEC59FFDBF74F7A157F48C24AA3053B7
        6152B76253B66152B66150B45E50B45E50B45E56B6655EAC65C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C7C7C7868686E1E1E1DFDFDF
        CECECEC3C3C37676769191919292929191919191918E8E8E8E8E8E8E8E8E9292
        928D8D8DD0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4B9CDC42B
        BA3A97FEC3B3FECF59F78F65AB6BC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0C8C8C8838383E0E0E0E7E7E7C5C5C58F8F8FD0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BBCEC72ABA399FFECB64FE9C6FB475C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C9C9C9838383
        E3E3E3CFCFCF999999D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4BDCEC82DBC3E70FEAB6FB775C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0CACACA868686D4D4D49B9B9BD0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BFCECA31BD3F6FBA76C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        CBCBCB8787879D9D9DD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C0CFCC84C08CC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0CCCCCCA9A9A9D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      NumGlyphs = 2
    end
    object BtnAvanti: TBitBtn
      Left = 108
      Top = 5
      Width = 102
      Height = 36
      Caption = '*Avanti'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 8
      OnClick = BtnAvantiClick
      OnMouseMove = AzzeraTimerMouseMove
      Glyph.Data = {
        B60D0000424DB60D000000000000360000002800000030000000180000000100
        180000000000800D0000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D482B68AC2CECDC8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0A2A2A2CCCCCCD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46BAC72249B2EC1CECCC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D09393936C6C6CCBCBCBD0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D46BAB723FC473259B
        2EBFCDCBC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D09292929C9C9C6C6C6CCACACAD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46BAA723CC26E3FC171249D2EBECDC9C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D09292929999999A9A9A6E6E6ECA
        CACAD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D464A26A41D07340CA
        7142BF7224A22EBDCDC8C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D08A8A8AA3A3A39F9F9F999999707070C9C9C9D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D457985C3F95483C94453B9444429A4D449C4F45
        9D50469E51469F521C8B2548E37C49E87F44CD7244C07327A331BCCDC7C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D07E7E7E7474747272
        727171717878787A7A7A7B7B7B7C7C7C7D7D7D5F5F5FB2B2B2B6B6B6A2A2A29A
        9A9A727272C9C9C9D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D446964E
        3FCB7736BE6739BF6A3AC06B3CBF6C3DBE6C3EBF6D3EBE6E3EBD6C3FC16B4BE6
        7F4DE98047D07647BF7529A734BACDC5C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0777777A1A1A1949494969696979797979797969696979797
        979797969696989898B5B5B5B7B7B7A5A5A59B9B9B767676C8C8C8D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D442924854E6893AD76F3DDA7240DC7442DE7644
        E17846E37A48E57C4AE77E4AE67E4CE67F4CE67F51ED854AD37949BF762AA935
        B8CDC3C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0727272B9B9B9A6A6
        A6A9A9A9ABABABADADADAFAFAFB1B1B1B3B3B3B5B5B5B5B5B5B5B5B5B5B5B5BC
        BCBCA8A8A89B9B9B777777C7C7C7D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4419148
        64E79439D56E3ED77140D97342DB7544DD7746DF7948E17B4AE37D4DE6804DE6
        804FE88250EA8354F0874ED67C4CBF782DAD3AB6CDC1C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0727272BFBFBFA4A4A4A7A7A7A9A9A9ABABABADADADAFAFAF
        B1B1B1B3B3B3B6B6B6B6B6B6B8B8B8B9B9B9BEBEBEABABAB9C9C9C7C7C7CC6C6
        C6D0D0D0D0D0D0D0D0D0C8D0D43E924572ECA03AD66D40D97342DB7544DD7746
        DF7948E17B4AE37D4CE57F4EE7814FE88251EA8454ED8755EF8858F48C51DC80
        4FC27D35B445B4CCBEC8D0D4C8D0D4D0D0D0D0D0D0D0D0D0717171C7C7C7A4A4
        A4A9A9A9ABABABADADADAFAFAFB1B1B1B3B3B3B5B5B5B7B7B7B8B8B8BABABABD
        BDBDBEBEBEC3C3C3B0B0B0A0A0A0848484C5C5C5D0D0D0D0D0D0C8D0D43E9345
        79EEA542D97441DA7443DC7645DE7848E17B4AE37D4CE57F4EE78150E98352EB
        8553EC8655EE8856EF8959F38C5FFE9556CD8638B54BB0CBBAC8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0717171CACACAA9A9A9AAAAAAACACACAEAEAEB1B1B1B3B3B3
        B5B5B5B7B7B7B9B9B9BBBBBBBCBCBCBEBEBEBFBFBFC2C2C2CCCCCCAAAAAA8787
        87C2C2C2D0D0D0D0D0D0C8D0D43E934578EFA64FDC7E3CDB7142DE7644E07845
        E27947E47B4BE77F4DE9814FEB8353ED8755EE8857F08A5AF38D5DF6926BFEA6
        33B341B3CDBDC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0717171CBCBCBAFAF
        AFA9A9A9ADADADAFAFAFB0B0B0B2B2B2B6B6B6B8B8B8BABABABCBCBCBEBEBEC0
        C0C0C3C3C3C6C6C6D2D2D2828282C5C5C5D0D0D0D0D0D0D0D0D0C8D0D43E9345
        7CEFA871E4985EE18961E38D67E7916AE9956BEB966FED9870EF9B75EF9E64F0
        9457F18B59F28C5EF7936BFEA537C248B5CCC0C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0717171CCCCCCC0C0C0B7B7B7BABABABEBEBEC1C1C1C2C2C2
        C5C5C5C7C7C7C8C8C8C4C4C4C0C0C0C2C2C2C7C7C7D2D2D28D8D8DC5C5C5D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D441964974F7A782F1AC85F2AF88F4B18CF6B48E
        F8B591F8B893FABA93FABAA3F9C075F49F58F38D5EF7936BFEA736BF45B7CCC2
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0757575D0D0D0CFCF
        CFD1D1D1D3D3D3D6D6D6D7D7D7D9D9D9DBDBDBDBDBDBDEDEDECCCCCCC2C2C2C7
        C7C7D3D3D38A8A8AC6C6C6D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4559B5B
        449F50429E4D419E4D4BA8594EAB5D4FAC5E50AE6050AF601D9F2D7FF7A865F4
        945CF7926BFEA634BA41B9CDC4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D07F7F7F7D7D7D7B7B7B7B7B7B8585858989898A8A8A8B8B8B
        8C8C8C6E6E6ED1D1D1C6C6C6C7C7C7D2D2D2868686C8C8C8D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D462A26883F9AC5BFA936AFEA431B840BBCDC6C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0898989D4D4D4C9C9C9D2D2D284
        8484C8C8C8D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D46BAB7276FEA869FE
        A32FB63EBCCDC7C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0929292D4D4D4D1D1D1828282C9C9C9D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46CAE726DFEA92DB23ABECDC9C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0949494D4D4D47E7E7ECACACAD0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D46CAF7230B43DBFCE
        CBC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0959595818181CBCBCBD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D481B989C0CECCC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0A4A4A4CBCBCBD0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      Layout = blGlyphRight
      NumGlyphs = 2
    end
    object BtnCerca: TBitBtn
      Left = 7
      Top = 40
      Width = 102
      Height = 36
      Caption = '*Cerca'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = BtnCercaClick
      OnMouseMove = AzzeraTimerMouseMove
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000EB0A0000EB0A00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D49093
        95757A7C7075776A70726D7072747879C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D09393
        937A7A7A7575756F6F6F707070777777D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D49BA0A1575D
        647F7571AE9486A0938E4F585E3A3B3D838789C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D09F9F9F5E5E
        5E7474749292929292925858583B3B3B878787D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4929597696F73A185
        71ECAD80DEA780E0A27ADCAC905963686B6E70C8D0D4C8D0D4C8D0D4C8D0D49D
        A1A27A7D7E72787A6D71736B6E70818586C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D09595956F6F6F8282
        82A6A6A6A1A1A19D9D9DA9A9A96363636E6E6ED0D0D0D0D0D0D0D0D0D0D0D0A0
        A0A07C7C7C7777777171716E6E6E848484D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D48B8E90666869737070FFCF
        94F9C894FCCA95F8C592F2B989D8B59D3C44497D8183C8D0D4C8D0D493969861
        62634952598A7D759D8F8872767E3B40407E8285C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D08E8E8E686868707070C2C2
        C2BEBEBEBFBFBFBBBBBBB1B1B1B1B1B1444444818181D0D0D0D0D0D096969662
        62625353537C7C7C8E8E8E7777773F3F3F828282D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D48B8F914B4B4C797F83B4977FFFDE
        A9FFD5AAFFD7ACFFD7AAFFD39FFFD296575C626F7274C8D0D47C8081686A6B56
        5D65D7A37DE6AC7FE0A87FDFA47EA3A0A03B4040777B7DC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D08F8F8F4B4B4B7F7F7F939393D1D1
        D1CCCCCCCECECECECECEC8C8C8C5C5C55D5D5D727272D0D0D07F7F7F6A6A6A5E
        5E5E9D9D9DA5A5A5A2A2A29F9F9FA0A0A03F3F3F7B7B7BD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4888C8E4849497979798F9599B69884FFE7
        BEFFE0C1FFE0C5FFDDBFFFD8B1FFE1A886796C44484A7073754F5050646A6CAD
        927AFFCF98FCCC96FBCB95F5C28FF3B98C636B706B6E6FC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D08C8C8C484848797979959595959595DDDD
        DDDADADADBDBDBD7D7D7D0D0D0D3D3D37676764848487373734F4F4F6969698D
        8D8DC4C4C4C1C1C1C0C0C0B8B8B8B2B2B26B6B6B6D6D6DD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D48A8D8F4C4D4E7B7B7B8D8D8D9FA3A59D8B83FFE5
        C9FFEFDEFFEFDDFFE9D5FFE0C1FFE9B880756C454748343434626161828790F1
        BE90FFDAADFFD8AEFFD7AFFFD5A7FFD399AD9D8D686D71C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D08D8D8D4D4D4D7B7B7B8D8D8DA3A3A38A8A8ADFDF
        DFEBEBEBEBEBEBE5E5E5DADADADCDCDC737373474747343434616161898989B6
        B6B6D0D0D0CFCFCFCFCFCFCCCCCCC6C6C69A9A9A6D6D6DD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D48A8D8F4C4D4E6666669898989E9E9EAFB1B1AAABACCCA3
        92FFFAF1FFFFFFFFF4E6FFE7CEFFF6C6484C502D2D2F5C5C5CA5A5A58C9498EA
        BE9DFFE6C8FFE2CBFFE3C9FFDCBBFFDDACD6B696697074C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D08D8D8D4D4D4D6666669898989E9E9EB0B0B0ABABABA2A2
        A2F7F7F7FFFFFFF1F1F1E2E2E2E8E8E84C4C4C2D2D2D5C5C5CA5A5A5949494B9
        B9B9DFDFDFDEDEDEDEDEDED6D6D6D2D2D2B0B0B0707070D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4878B8D5353544A4A4A707070ACACACB0B0B0C1C0C0CDD1D29587
        85C8A9A0F3E6E5FFF2E5FFF6D7AC93823F45495D5D5CA9A9A9ACACABA8ABAFC0
        A392FFF4DCFFF3E5FFEDDEFFE5CBFFE5BBCAAB8F6E7377C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D08B8B8B5353534A4A4A707070ACACACB0B0B0C0C0C0D0D0D08787
        87A9A9A9E7E7E7EFEFEFEDEDED9090904545455C5C5CA9A9A9ABABABABABABA1
        A1A1EEEEEEF0F0F0EAEAEAE0E0E0DBDBDBA6A6A6737373D0D0D0C8D0D4C8D0D4
        C8D0D4888B8D4243435252525D5D5D4F4F4FC4C4C4C8C8C8D1D1D1E8E8E8C6C9
        C97E777695756EA57F76826A64565B5F2B2C2C8B8B8BB4B4B4BBBBBBC3C5C6A8
        9FA0E1C4B4FFFFFFFFFFFCFFEEDCFFFAD07D756B787D7FC8D0D4D0D0D0D0D0D0
        D0D0D08B8B8B4242425252525D5D5D4F4F4FC4C4C4C8C8C8D1D1D1E8E8E8C8C8
        C87777777676768080806A6A6A5B5B5B2B2B2B8B8B8BB4B4B4BBBBBBC5C5C5A0
        A0A0C2C2C2FFFFFFFEFEFEEAEAEAEEEEEE7272727D7D7DD0D0D0C8D0D4C8D0D4
        868A8C3A3B3B9A9A9AFFFFFF6E6E6E4949498C8C8CE5E5E5EAEAEAF9F9F9FFFF
        FFFEFFFF8B8E90777B7D7171736667674848484F4F4FCECECECCCCCCE1E1E2CC
        D1D2988381CAACA4EFDACFFFE3CBBC9E8A5A6265909496C8D0D4D0D0D0D0D0D0
        8A8A8A3A3A3A9A9A9AFFFFFF6E6E6E4949498C8C8CE5E5E5EAEAEAF9F9F9FFFF
        FFFEFEFE8E8E8E7B7B7B7171716666664848484F4F4FCECECECCCCCCE1E1E1D0
        D0D0848484ADADADD9D9D9DFDFDF9B9B9B616161949494D0D0D0C8D0D4818486
        454647717171FFFFFFAEAEAE8C8C8CBDBDBD3D3D3D868686F2F2F2FFFFFFF4F4
        F47675753D3C3C4747478181817E7E7E4A4A4A626262E1E1E1DDDDDDEEEEEEFF
        FFFFCAD0D08F898A967F7E7B6B6B6D7276A1A5A7C8D0D4C8D0D4D0D0D0848484
        464646717171FFFFFFAEAEAE8C8C8CBDBDBD3D3D3D868686F2F2F2FFFFFFF4F4
        F47575753C3C3C4747478181817E7E7E4A4A4A626262E1E1E1DDDDDDEEEEEEFF
        FFFFCFCFCF8989898181816C6C6C727272A5A5A5D0D0D0D0D0D0C8D0D4727576
        3939397070708B8B8B767676BFBFBFD3D3D3D7D7D76C6C6C4A4A4A7777773838
        383C3C3C5454546262625353535B5B5B2D2D2DCACACAFAFAFAFFFFFFFFFFFFFF
        FFFFFFFFFF8D8E8E575B5C909597ABAFB1C8D0D4C8D0D4C8D0D4D0D0D0747474
        3939397070708B8B8B767676BFBFBFD3D3D3D7D7D76C6C6C4A4A4A7777773838
        383C3C3C5454546262625353535B5B5B2D2D2DCACACAFAFAFAFFFFFFFFFFFFFF
        FFFFFFFFFF8D8D8D5A5A5A959595AFAFAFD0D0D0D0D0D0D0D0D0C8D0D476797A
        434343787878B8B8B8C5C5C5CFCFCFC7C7C7C2C2C2CACACAB9B9B98D8D8D7676
        765F5F5F4B4B4B454545383838363636555555787878A7A7A7C2C2C2ADADAD84
        84842E2F30636668818486C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0787878
        434343787878B8B8B8C5C5C5CFCFCFC7C7C7C2C2C2CACACAB9B9B98D8D8D7676
        765F5F5F4B4B4B454545383838363636555555787878A7A7A7C2C2C2ADADAD84
        84842F2F2F666666848484D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4797C7E
        5959593A3A3AAEAEAEE3E3E3C9C9C9BFBFBFB9B9B9B3B3B3B1B1B1AFAFAFAFAF
        AFB0B0B0A8A8A89898988A8A8A7E7E7E7272726060605E5E5E3D3D3D33343467
        6A6B828587C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D07C7C7C
        5959593A3A3AAEAEAEE3E3E3C9C9C9BFBFBFB9B9B9B3B3B3B1B1B1AFAFAFAFAF
        AFB0B0B0A8A8A89898988A8A8A7E7E7E7272726060605E5E5E3D3D3D33333369
        6969858585D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D48C9092
        6162626666664242426E6E6EB1B1B1CACACABFBFBFB4B4B4AEAEAEA4A4A49E9E
        9E9999999999999A9A9A9E9E9EA0A0A0ACACACB0B0B0646464424343838688C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0909090
        6161616666664242426E6E6EB1B1B1CACACABFBFBFB4B4B4AEAEAEA4A4A49E9E
        9E9999999999999A9A9A9E9E9EA0A0A0ACACACB0B0B0646464424242868686D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        9CA0A1696A6A5E5E5E62626386898A686969747474838383939393A0A0A0A6A6
        A6A7A7A7A6A6A6A6A6A6A6A6A6AEAEAE7878783F40406E7173868A8CC8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        9F9F9F6969695E5E5E626262888888686868747474838383939393A0A0A0A6A6
        A6A7A7A7A6A6A6A6A6A6A6A6A6AEAEAE7878783F3F3F7171718A8A8AD0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D485898B929597A4A8AAC8D0D4A5A9AB9093958D909186898B8184855657
        575151515757575B5B5B5858584F5050707375898C8EC8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0898989959595A8A8A8D0D0D0A9A9A99393938F8F8F8989898383835656
        565151515757575B5B5B5858584F4F4F7373738C8C8CD0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D49B9F
        A18B8E8F8B8E90888B8D81848584888AC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D09F9F
        9F8D8D8D8E8E8E8B8B8B838383888888D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      Margin = 2
      NumGlyphs = 2
    end
    object BtnSalva: TBitBtn
      Left = 7
      Top = 75
      Width = 102
      Height = 36
      Caption = '*Salva'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      OnClick = BtnSalvaClick
      OnMouseMove = AzzeraTimerMouseMove
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000C30E0000C30E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4E3E1E0C2967CB07654AE7254AB6E5192979AA4A5A5A6A7A9A9AB
        ADADAEB0AFB1B2B1B1B2ACACADA7A7A8A2A2A39D9D9D979899929596896E62AD
        6F51AB6F53AB6E53AB6E52AE745AC6A596E4E6E8C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0E0E0E09393937272726F6F6F6C6C6C979797A4A4A4A7A7A7ABAB
        ABAEAEAEB1B1B1B1B1B1ACACACA7A7A7A2A2A29D9D9D9898989494946D6D6D6D
        6D6D6D6D6D6C6C6C6C6C6C727272A4A4A4E6E6E6D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4E3E2E0BF723DB85E13CA7D2CCA7C2BC87927878E95C0BCB7CEB69BD7C2
        A9E2CCB4EDD6B8FEFEFEFFFFFFF7F7F8EAEAEBDCDCDDCFCFCFC5C9CD7C5C34DE
        9436D89138D99237E1A759DA943AA9490BC9AA9CC8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0E1E1E16A6A6A5151516D6D6D6C6C6C6969698F8F8FBABABAB0B0B0BCBC
        BCC7C7C7CFCFCFFEFEFEFFFFFFF7F7F7EAEAEADCDCDCCFCFCFC9C9C953535380
        80807E7E7E7E7E7E969696818181414141A9A9A9D0D0D0D0D0D0C8D0D4C8D0D4
        E3E1DEC67A3FBC6214C6782AC67627C67528C67525808891B1A090CC751ACB7B
        24CF8027D4821EE6E6E3F4F5F6FDFDFDF1F1F1E3E3E3D6D6D6CED2D57C5A34DA
        8D33D38A34D48A30E8BE8CD9933CD78F36B07A61C8D0D4C8D0D4D0D0D0D0D0D0
        E0E0E07171715454546969696767676767676666668989899D9D9D6363636A6A
        6A6E6E6E6D6D6DE5E5E5F5F5F5FDFDFDF1F1F1E3E3E3D6D6D6D2D2D25252527A
        7A7A787878777777B3B3B38080807C7C7C787878D0D0D0D0D0D0C8D0D4C8D0D4
        CA986BBF6513C37428C37327C37226C37326C47123848C94A59484C16A1AC271
        25C7782ACD7B21DAD9D7E6E6E7F4F4F4FDFDFDF2F2F2E4E4E4DCDFE37C5A34D8
        8A32D28833D3872EE6BC87D79139D99238AD735AC8D0D4C8D0D4D0D0D0D0D0D0
        9090905656566666666565656464646565656363638D8D8D9191915B5B5B6363
        63696969696969D8D8D8E6E6E6F4F4F4FDFDFDF2F2F2E4E4E4DFDFDF52525278
        7878777777757575B0B0B07E7E7E7F7F7F727272D0D0D0D0D0D0C8D0D4C8D0D4
        C9915CC16F25C17025C16F24C17024C37025C06E21929AA2A9988BB95C12BA65
        1EC06D23C7721CCECDCBDADBDCE6E6E6F3F3F3FDFDFDF2F2F2ECEFF37D5A36D5
        8830D18732D2852DE5B986D68E38D88F36AD735AC8D0D4C8D0D4D0D0D0D0D0D0
        8787876262626262626161616262626363636060609B9B9B9696965050505959
        59606060616161CCCCCCDBDBDBE6E6E6F3F3F3FDFDFDF2F2F2EFEFEF53535376
        7676767676737373AEAEAE7C7C7C7C7C7C727272D0D0D0D0D0D0C8D0D4C8D0D4
        CD955CBF6B21BF6C22BF6D23BF6D23C06E23BF6C1F9DA5ADB3A094AE4B08B35A
        17BA641EC36B19C1C0BFCDCECFD9D9D9E5E5E5F3F3F3FDFDFDFAFEFF7D5A35D4
        862FCF8531D1842CE4B887D58C37D58E35AD735AC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A5E5E5E5F5F5F6060606060606060605E5E5EA6A6A69E9E9E4242425050
        505858585C5C5CBFBFBFCECECED9D9D9E5E5E5F3F3F3FDFDFDFDFDFD52525274
        7474747474727272AEAEAE7A7A7A7B7B7B727272D0D0D0D0D0D0C8D0D4C8D0D4
        CE965CBE6920BD6A22BD6A21BE6A21BE6B22BD681EA8B1B9BCA99EA23B00AD4F
        11B65E19C16616B5B1B0BFBFC0CCCCCCD8D8D8E5E5E5F2F2F2FFFFFF7E5C36D3
        832ECE8330CE822BE3B886D48A36D48C34AD735AC8D0D4C8D0D4D0D0D0D0D0D0
        8B8B8B5C5C5C5D5D5D5D5D5D5D5D5D5E5E5E5B5B5BB2B2B2A7A7A73535354747
        47535353585858B1B1B1BFBFBFCCCCCCD8D8D8E5E5E5F2F2F2FFFFFF54545472
        7272727272707070ADADAD797979797979727272D0D0D0D0D0D0C8D0D4C8D0D4
        CE955CBB671DBB671FBC671FBC6820BC6920B9661AB0B9C1C6B2A89F3D05AB52
        18B35F1FBC691EA6A5A1B2B3B4C0C0C0CBCBCBD9D9D9E5E5E5FAFEFF805C38D5
        832ED0832FCF822BE5B888D28A36D38B33AD735AC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A5A5A5A5B5B5B5B5B5B5C5C5C5C5C5C585858BABABAB1B1B13737374A4A
        4A5555555C5C5CA3A3A3B3B3B3C0C0C0CBCBCBD9D9D9E5E5E5FDFDFD55555572
        7272727272707070AEAEAE797979787878727272D0D0D0D0D0D0C8D0D4C8D0D4
        CE955CBB651CBA651FBA651EBA661EBC661FB8621ABBC6CFE4EBEFD3DEE5C5D0
        D8B3BEC7A5AEB79A9EA3A4A7AAB3B7BAC0C3C6CFD2D6DCDFE3F1F7FE715537B3
        6D25B06F28AF6E26D79F60D18833D28932AD735AC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A5858585959595959595A5A5A5A5A5A565656C7C7C7EBEBEBDEDEDED1D1
        D1BFBFBFAFAFAF9F9F9FA7A7A7B7B7B7C3C3C3D2D2D2DFDFDFF8F8F84F4F4F5F
        5F5F6161615F5F5F929292767676777777727272D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BB8611CB8621CB8631CB9631DB9641DBA641D9A765AA07A5AA57E5CA57F
        5FA27E5CA07B599C775598724F916D4B8F694688634183603D7F5D3B91602EAA
        6824A86926A76926CA7F2CCF8431D18832AD745AC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A5656565656565757575757575858585858587171717474747878787979
        797777777575757171716B6B6B6666666262625D5D5D5959595656565656565B
        5B5B5C5C5C5C5C5C6E6E6E737373767676727272D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BB56019B6601CB7601BB7611BB8611CB9621CBC631ABB6318BC6319BE64
        1ABF671BC0681DC16B1FC36D20C46E21C77022C87324CA7625CB7827CC7A29CB
        7C2BCD7E2CCE802DCD812ECE8430D08630AD745AC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A5454545555555555555555555656565656565757575656565757575858
        585A5A5A5B5B5B5E5E5E5F5F5F6060606262626565656767676969696B6B6B6C
        6C6C6E6E6E707070707070737373747474727272D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BB55C19B55D19B55D19B55E1AB65F1AB6601BB7611BB8621CB9631DBA64
        1EBB661EBB671FBD6920BE6B21BF6C22C06E24C27025C37226C47427C67628C7
        782AC87A2BCA7D2CCB7F2DCD812FD18531AD745AC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A5252525252525252525353535454545555555555555656565757575858
        585A5A5A5B5B5B5C5C5C5E5E5E5F5F5F61616162626264646466666667676769
        69696B6B6B6D6D6D6F6F6F717171747474727272D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BB25A16B35A19B35B18B45B15B45812B55810B75911B65C13B75D12B85E
        13BA6114BB6116BC6417BD6618C06819C1691AC26B1DC56D1EC5711FC87320C9
        7623CA7824CB7B28CA7D2DCC812ECF842FAD745AC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A4F4F4F5050505050505050504D4D4D4D4D4D4E4E4E5050505050505151
        515454545454545757575858585A5A5A5B5B5B5D5D5D5F5F5F62626263636366
        66666868686B6B6B6D6D6D707070737373727272D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BB05615B15816B25713B16F3EAF9784AD927EAE937DAE937DAE947FAE94
        7EAE947CAE937CAE937DAD947BAD927CAC937AAA9179AB917AA98F78A89077A8
        8F76A78F7BB9844FCC7C29CB7E2DCE832EAD745BC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A4C4C4C4E4E4E4D4D4D6767679494948F8F8F8F8F8F8F8F8F9090909090
        908F8F8F8F8F8F8F8F8F8F8F8F8E8E8E8E8E8E8C8C8C8D8D8D8B8B8B8B8B8B8A
        8A8A8B8B8B7A7A7A6C6C6C6E6E6E727272737373D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BB05513B05516B05411B07851BFC8CDD7D9DCD6D9DBD8DADCD7DADCD7D9
        DBD8DADCD6D8DAD5D7D9D4D7D9D1D4D6CFD2D4CDD0D2CBCDD0C9CBCEC7C9CCC7
        CACCAFB4BBB18A61CB7A27CA7D2CCC802DAD745BC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A4B4B4B4C4C4C4A4A4A727272C8C8C8D9D9D9D9D9D9DADADADADADAD9D9
        D9DADADAD8D8D8D7D7D7D7D7D7D4D4D4D2D2D2D0D0D0CDCDCDCBCBCBC9C9C9CA
        CACAB5B5B58282826A6A6A6D6D6D6F6F6F737373D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BAC5112AE5313AE500EAF764EC8CED1E9E9E9E9E9E9E9E9E9EBEBEBECEC
        ECEDEDEDEBEBEBEAEAEAE9E9E9E7E7E7E4E4E4E2E2E2E0E0E0DDDDDDDADADAD9
        D9D9BABDC1B0885ECA7A25C97C2DCB7F2EAD755BC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A4848484A4A4A464646707070CECECEE9E9E9E9E9E9E9E9E9EBEBEBECEC
        ECEDEDEDEBEBEBEAEAEAE9E9E9E7E7E7E4E4E4E2E2E2E0E0E0DDDDDDDADADAD9
        D9D9BDBDBD8080806969696D6D6D6F6F6F737373D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BAC4E10AC5011AD4E0DAF764DCBD0D4EBEBEBEAEAEAEEEEEED2D2D2CECE
        CECECECECECECECECECECECECECFCFCFCDCDCDCECECECDCDCDDCDCDCDADADADA
        DADABABDC1B08860C97726CA7B2BCC7E2CAD755BC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A464646474747454545707070D0D0D0EBEBEBEAEAEAEEEEEED2D2D2CECE
        CECECECECECECECECECECECECECFCFCFCDCDCDCECECECDCDCDDCDCDCDADADADA
        DADABDBDBD8080806868686C6C6C6E6E6E737373D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BA94D0FAB4E10AB4C0AAE754DCED4D7EEEEEEEDEDEDF1F1F1CFCFCFC9C9
        C9CACACACACACACACACACACACACACACACBCBCBCBCBCBCACACADDDDDDDDDDDDDC
        DCDCBDC0C3B2885FC87624C87A2ACA7D2DAD755BC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A4444444646464343436F6F6FD4D4D4EEEEEEEDEDEDF1F1F1CFCFCFC9C9
        C9CACACACACACACACACACACACACACACACBCBCBCBCBCBCACACADDDDDDDDDDDDDC
        DCDCC0C0C08080806666666A6A6A6D6D6D737373D0D0D0D0D0D0C8D0D4C8D0D4
        CE965BA7490DA94B0EA9490AAE754DD0D6D9F1F1F1F0F0F0F4F4F4EEEEEEECEC
        ECEDEDEDEBEBEBEAEAEAE8E8E8E6E6E6E5E5E5E1E1E1DFDFDFE1E1E1DDDDDDDD
        DDDDBDC1C4B1875FC97523C7792BC97E2BAD755BC8D0D4C8D0D4D0D0D0D0D0D0
        8A8A8A4141414343434141416F6F6FD6D6D6F1F1F1F0F0F0F4F4F4EEEEEEECEC
        ECEDEDEDEBEBEBEAEAEAE8E8E8E6E6E6E5E5E5E1E1E1DFDFDFE1E1E1DDDDDDDD
        DDDDC1C1C17F7F7F6666666A6A6A6D6D6D737373D0D0D0D0D0D0C8D0D4C8D0D4
        CF975CA7470CA7490DA84707B0754FD4D9DDF4F4F4F3F3F3F7F7F7E7E7E7E6E6
        E6E7E7E7E5E5E5E4E4E4E3E3E3E0E0E0E0E0E0DDDDDDDADADAE3E3E3E0E0E0DF
        DFDFBEC2C5B18761C77324C87829CA7C2CAD755BC8D0D4C8D0D4D0D0D0D0D0D0
        8B8B8B4040404141413E3E3E707070D9D9D9F4F4F4F3F3F3F7F7F7E7E7E7E6E6
        E6E7E7E7E5E5E5E4E4E4E3E3E3E0E0E0E0E0E0DDDDDDDADADAE3E3E3E0E0E0DF
        DFDFC2C2C28080806464646969696C6C6C737373D0D0D0D0D0D0C8D0D4C8D0D4
        CF975CA4440AA6460BA64406AF744ED5DBDEF7F7F7F6F6F6FBFBFBD0D0D0C9C9
        C9C9C9C9C9C9C9C9C9C9CACACACACACACACACACBCBCBC9C9C9E2E2E2E0E0E0E0
        E0E0C0C3C6B18760C77422C6772AC87B2AAD755BC8D0D4C8D0D4D0D0D0D0D0D0
        8B8B8B3D3D3D3F3F3F3C3C3C6F6F6FDBDBDBF7F7F7F6F6F6FBFBFBD0D0D0C9C9
        C9C9C9C9C9C9C9C9C9C9CACACACACACACACACACBCBCBC9C9C9E2E2E2E0E0E0E0
        E0E0C3C3C38080806464646868686B6B6B737373D0D0D0D0D0D0C8D0D4C8D0D4
        CF975CA34209A4440AA54204AF744ED9DFE2F9F9F9F9F9F9FEFEFEDDDDDDD9D9
        D9DADADAD8D8D8D7D7D7D8D8D8D7D7D7D6D6D6D5D5D5D3D3D3E3E3E3E2E2E2E0
        E0E0C1C4C7B18760C67222C77828C87A2BAD755BC8D0D4C8D0D4D0D0D0D0D0D0
        8B8B8B3C3C3C3D3D3D3A3A3A6F6F6FDFDFDFF9F9F9F9F9F9FEFEFEDDDDDDD9D9
        D9DADADAD8D8D8D7D7D7D8D8D8D7D7D7D6D6D6D5D5D5D3D3D3E3E3E3E2E2E2E0
        E0E0C4C4C48080806363636969696B6B6B737373D0D0D0D0D0D0C8D0D4C8D0D4
        CF985CA14007A24108A34003B0734EDCE1E5FBFBFBFCFCFCFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFCFCFCF8F8F8F4F4F4F1F1F1ECECECE6E6E6E1E1E1E1
        E1E1C2C5C9B38860C77323C77829C87A29AE765BC8D0D4C8D0D4D0D0D0D0D0D0
        8C8C8C3A3A3A3B3B3B3939396E6E6EE1E1E1FBFBFBFCFCFCFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFCFCFCF8F8F8F4F4F4F1F1F1ECECECE6E6E6E1E1E1E1
        E1E1C5C5C58080806464646969696A6A6A747474D0D0D0D0D0D0C8D0D4C8D0D4
        CF985C9F3C06A13F07A23C02AE734EDCE2E5FCFCFCFDFDFDFFFFFFDCDCDCD6D6
        D6D6D6D6D7D7D7D6D6D6D5D5D5D4D4D4D3D3D3D4D4D4D1D1D1E5E5E5E3E3E3E1
        E1E1C2C5C8B48961B4671EB56B24CB7B29B2795BC8D0D4C8D0D4D0D0D0D0D0D0
        8C8C8C3737373939393636366E6E6EE2E2E2FCFCFCFDFDFDFFFFFFDCDCDCD6D6
        D6D6D6D6D7D7D7D6D6D6D5D5D5D4D4D4D3D3D3D4D4D4D1D1D1E5E5E5E3E3E3E1
        E1E1C5C5C58181815959595E5E5E6B6B6B767676D0D0D0D0D0D0C8D0D4C8D0D4
        D0985C9E3A059F3D06A03A01AE724FDEE4E7FCFCFCFCFCFCFFFFFFCFCFCFC7C7
        C7C9C9C9C9C9C9C9C9C9C9C9C9C9C9C9CACACACACACAC9C9C9E4E4E4E3E3E3E1
        E1E1C2C5C8B78A627C4613824E1CCC7B2AB57C5CC8D0D4C8D0D4D0D0D0D0D0D0
        8C8C8C3535353737373434346E6E6EE4E4E4FCFCFCFCFCFCFFFFFFCFCFCFC7C7
        C7C9C9C9C9C9C9C9C9C9C9C9C9C9C9C9CACACACACACAC9C9C9E4E4E4E3E3E3E1
        E1E1C5C5C58383833C3C3C4444446C6C6C787878D0D0D0D0D0D0C8D0D4C8D0D4
        D09B639E3A039E3B049E3900AE704DDEE4E7FBFBFBFBFBFBFFFFFFF4F4F4F4F4
        F4F4F4F4F2F2F2F0F0F0ECECECEBEBEBE7E7E7E5E5E5E1E1E1E6E6E6E1E1E1E1
        E1E1C2C5C9B38A62A15C19A66321C87728BA8461C8D0D4C8D0D4D0D0D0D0D0D0
        9090903434343535353333336C6C6CE4E4E4FBFBFBFBFBFBFFFFFFF4F4F4F4F4
        F4F4F4F4F2F2F2F0F0F0ECECECEBEBEBE7E7E7E5E5E5E1E1E1E6E6E6E1E1E1E1
        E1E1C5C5C58282824F4F4F5656566868687F7F7FD0D0D0D0D0D0C8D0D4C8D0D4
        DBBC9DBF59009E39039A3300AD6E4DDDE3E6FBFBFBFCFDFDFFFFFFFFFFFFFFFF
        FFFFFFFFFEFEFEFCFCFDF9F9F9F6F6F6F2F2F2EFEFEFECECECE7E7E7E3E4E4E2
        E3E3C2C7CAB28862C87224C77727B75306CFB39DC8D0D4C8D0D4D0D0D0D0D0D0
        B6B6B64A4A4A3434342F2F2F6B6B6BE3E3E3FBFBFBFCFCFCFFFFFFFFFFFFFFFF
        FFFFFFFFFEFEFEFCFCFCF9F9F9F6F6F6F2F2F2EFEFEFECECECE7E7E7E3E3E3E2
        E2E2C7C7C7818181646464686868474747AFAFAFD0D0D0D0D0D0C8D0D4C8D0D4
        E4E8E8DABB9BCF9A60CD9050CBA680C8CCCFC8C9C9C8CACAC9CACAC8C9C9C7C7
        C8C4C7C6C4C4C4C1C4C3BFC1C1BFBFBFBCBFBEBABCBCB8BABAB7B8B9B6B6B7B4
        B6B5B2B6B9BC9F83C4874FC38C5FD1B39BE4E7E8C8D0D4C8D0D4D0D0D0D0D0D0
        E7E7E7B4B4B48E8E8E8383839E9E9ECCCCCCC8C8C8C9C9C9C9C9C9C8C8C8C7C7
        C7C6C6C6C4C4C4C3C3C3C0C0C0BFBFBFBEBEBEBBBBBBB9B9B9B8B8B8B6B6B6B5
        B5B5B6B6B69999997D7D7D848484AFAFAFE6E6E6D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      Margin = 2
      NumGlyphs = 2
    end
    object BtnCancella: TBitBtn
      Left = 7
      Top = 110
      Width = 102
      Height = 36
      Caption = '*Cancella'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
      OnClick = BtnCancellaClick
      OnMouseMove = AzzeraTimerMouseMove
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000EB0A0000EB0A00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        616AC74A4DBEBFB5B1AD9E979D8A819D8980A6958DBFB5B0BFB5B0BEB5B0A99A
        929C89809C897F9C897F9C897F9B897F9B887F9A887F9A877F9A877E9A877F99
        877E99877E98867E99877F9E918AC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        8484846E6E6EB4B4B49D9D9D898989888888949494B4B4B4B4B4B4B4B4B49999
        9988888888888888888888888888888887878787878786868686868686868686
        8686868686858585868686909090D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4616AC5
        243FD41E4EFA0203A68A84C9F2E5DFF5E5D7B9B0D7040CB2020DA902029BA0A1
        D8F4E4D4F2DDCCF2DDCBF2DDCBF2DECAF2DCCAF2DCC7F3DBC7F1DAC4F1D9C2F2
        D8C0F0D7BEF1D6BDF5D9BD98867EC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0848484
        6868687B7B7B333333999999E4E4E4E2E2E2BCBCBC3C3C3C3A3A3A2F2F2FB1B1
        B1E1E1E1DADADADADADADADADADADADAD9D9D9D8D8D8D7D7D7D6D6D6D4D4D4D3
        D3D3D2D2D2D1D1D1D3D3D3858585D0D0D0D0D0D0D0D0D0D0D0D06367D12B2CAA
        2150F72360FF215DFF0919BD4140A9B7B4DB0308AC1340EA144FFF0F4AFF0403
        A6FCF9F2FAF2E8FAF2E7FAF2E7FAF3E6FAF1E6FAF2E5FAF2E4F9F1E2F9F0E0F9
        EFDEF8ECDBF9EDD9F4DCC497857CC8D0D4C8D0D4C8D0D4C8D0D4868686515151
        7C7C7C8888888686864848485F5F5FC0C0C03838386D6D6D7C7C7C7979793333
        33F7F7F7EFEFEFEFEFEFEFEFEFEFEFEFEEEEEEEFEFEFEEEEEEEDEDEDECECECEB
        EBEBE8E8E8E8E8E8D7D7D7848484D0D0D0D0D0D0D0D0D0D0D0D0C8D0D45D61BD
        2438C92860FF2358FF245EFF0D22C70000991744EA174DFF1349FF1142EA0415
        B6FDFAF3FBF4EBFBF4E9FBF4EAFBF5E8FBF3E9FBF3E7FCF4E6FAF2E5FAF1E5F9
        F0E1F9EFE0FAF0DEF4DEC698867DC8D0D4C8D0D4C8D0D4C8D0D4D0D0D07C7C7C
        6161618989898383838787875050502D2D2D7070707C7C7C7979796E6E6E4343
        43F8F8F8F2F2F2F1F1F1F1F1F1F1F1F1F0F0F0F0F0F0F0F0F0EFEFEFEEEEEEEC
        ECECEBEBEBEBEBEBD9D9D9858585D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        4F56C01F49EA2A61FF2459FF235AFF1A44EA1D51FF1A4FFF1643EA071BC25D58
        B1E6D3C9DDC2B3DDC1B2DDC1B0DDBFAEDDBEABDDBDAADDBCA8DDBAA5DDBAA5DF
        BEA9FAF1E2FAF0E1F3DFC998867DC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        7474747474748989898484848585857070707F7F7F7D7D7D6F6F6F4A4A4A7373
        73D2D2D2C0C0C0BFBFBFBFBFBFBDBDBDBBBBBBBABABAB9B9B9B7B7B7B7B7B7BB
        BBBBEDEDEDECECECDADADA858585D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        5D61C516179B1938DB2C63FF2557FF2254FF1F53FF1F55FF050CAC534EAEE2D1
        CEDCC0B3DBBEB1DBBDAEDABBACDABAAADAB9A8DAB7A5DAB6A2DAB5A0DAB5A0DD
        BAA4FAF2E5FBF3E4F5E0CC98877EC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        7E7E7E3E3E3E6565658B8B8B8383838181818080808181813B3B3B6B6B6BD2D2
        D2BFBFBFBDBDBDBBBBBBBABABAB8B8B8B7B7B7B5B5B5B4B4B4B2B2B2B2B2B2B7
        B7B7EFEFEFEFEFEFDCDCDC868686D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D45D61BB2D2C8E1E42E5285DFF2557FF2458FF1737DC292795E8DAD4DCC3
        B7DBC0B5DBBFB2DBBEB1DBBDAFDBBCACDABAAADAB9A6DAB7A4DAB6A1DAB6A1DD
        BBA5FBF3E6FBF4E7F4E2CE99867EC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D07B7B7B4949496E6E6E878787838383848484646464484848D9D9D9C2C2
        C2BFBFBFBEBEBEBDBDBDBCBCBCBABABAB8B8B8B7B7B7B5B5B5B3B3B3B3B3B3B8
        B8B8F0F0F0F0F0F0DEDEDE858585D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D44F51A73268FF2D62FF2A60FF275DFF255EFF0305A1948CC7E2CF
        C7DBC2B8DBC0B6DBBFB3DBBEB1DBBDAEDBBCACDAB9A8DAB8A5DAB7A3DAB7A3DD
        BBA7FBF3E7FCF5E8F6E3D29A877EC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D06A6A6A8E8E8E8A8A8A8989898787878787873333339E9E9ECECE
        CEC1C1C1C0C0C0BEBEBEBDBDBDBBBBBBBABABAB7B7B7B6B6B6B5B5B5B5B5B5B8
        B8B8F0F0F0F1F1F1E0E0E0868686D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D45D61BF222EB13871FF356EFF142CCC2556FD2962FF2054FF0D0C97CEC2
        D7DDC6BEDBC1B7DBC0B5DBBEB3DBBDAFDBBCACDBBBAADBBAA8DAB7A4DAB7A4DD
        BDA8FBF4E8FCF4E9F5E4D39A877FC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D07C7C7C5353539494949292925959598282828A8A8A818181353535C9C9
        C9C6C6C6C0C0C0BFBFBFBEBEBEBCBCBCBABABAB9B9B9B8B8B8B5B5B5B5B5B5BA
        BABAF1F1F1F1F1F1E0E0E0868686D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D44A4DB4284FEA3C75FF1B37D42F2E9A0605921D47EA2863FF1333DE5651
        ABE5D4CFDBC3BADBC1B7DBBFB3DBBEB1DBBCAFDBBBABDBBAA8DBB8A5DBB8A5DD
        BDAAFBF4E9FCF6EAF5E4D29A887EC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D06B6B6B7878789797976262624E4E4E2F2F2F7272728A8A8A6262626C6C
        6CD4D4D4C3C3C3C0C0C0BEBEBEBDBDBDBBBBBBB9B9B9B8B8B8B6B6B6B6B6B6BA
        BABAF1F1F1F3F3F3E0E0E0878787D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D44D52B73666EA2A54EA101098E4E2F0EBE0DD02029F1E4AEA2669FF040B
        A6EBDEDADBC3BADBC2B8DBC0B5DBBEB2DBBDAFDBBBACDBBAA9DBB9A7DBB9A7DE
        BEAAFBF5EAFCF5EBF5E4D39C877EC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D06F6F6F8787877C7C7C383838E6E6E6E0E0E03030307474748E8E8E3838
        38DEDEDEC3C3C3C1C1C1BFBFBFBDBDBDBCBCBCBABABAB8B8B8B7B7B7B7B7B7BB
        BBBBF2F2F2F2F2F2E0E0E0868686D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D45D61B63146D1101EBB8F8ED7FDFCF9DBC7C3EBE0DE070DB41C48F80610
        B8EBDEDBDBC4BBDBC2B8DBC0B5DBBFB3DBBDAFDBBCACDBBAAADBB8A6DBB8A6DE
        BEABFBF5EBFCF5ECF5E4D49B897FC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D07979796D6D6D4B4B4BA3A3A3FBFBFBC8C8C8E0E0E03E3E3E7777774141
        41DEDEDEC3C3C3C1C1C1BFBFBFBEBEBEBCBCBCBABABAB8B8B8B6B6B6B6B6B6BB
        BBBBF2F2F2F3F3F3E1E1E1888888D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D48C87B93737BCF9F8FAFDFBF8DCC7C2DBC6C3EBDFDD3E3BBE7C76
        B8DFCECADBC3BBDBC1B9DBC0B5DBBFB2DBBEB0DBBCADDBBAAADBB8A7DBB8A7DE
        BDABFBF5ECFCF7EDF5E5D69C887FC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D09696965E5E5EF8F8F8FAFAFAC7C7C7C7C7C7DFDFDF6262628A8A
        8ACECECEC3C3C3C1C1C1BFBFBFBEBEBEBDBDBDBBBBBBB8B8B8B6B6B6B6B6B6BB
        BBBBF2F2F2F4F4F4E2E2E2878787D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4B3A29AFFFFFFFEFCF9FEFCFADCC6C0DCC6C0DBC5BFE7D9D5E3D1
        CCDCC4BDDBC3BADBC2B7DBC0B5DBBEB2DBBDAFDBBCADDBBAAADBB9A7DBB9A7DE
        BDABFCF6EDFCF6EDF5E5D59C897FC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0A1A1A1FFFFFFFBFBFBFBFBFBC6C6C6C6C6C6C5C5C5D9D9D9D1D1
        D1C4C4C4C3C3C3C1C1C1BFBFBFBDBDBDBCBCBCBBBBBBB8B8B8B7B7B7B7B7B7BB
        BBBBF3F3F3F3F3F3E2E2E2888888D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4A69084FFFFFFFEFCFBFEFDFBDCC6C0DCC6C0DCC6C0DCC6BEDCC4
        BEDCC4BBDCC3BADBC1B7DBC0B4DBBEB2DBBDAFDBBCADDBBBAADBB9A7DBB9A7DE
        BDAAFCF6EDFCF6EEF5E5D79D8980C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D08E8E8EFFFFFFFBFBFBFCFCFCC6C6C6C6C6C6C6C6C6C6C6C6C4C4
        C4C4C4C4C3C3C3C0C0C0BFBFBFBDBDBDBCBCBCBBBBBBB9B9B9B7B7B7B7B7B7BB
        BBBBF3F3F3F4F4F4E2E2E2888888D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4A79185FFFFFFFEFDFCFEFDFDFEFEFDFEFEFDFEFEFDFEFDFDFEFD
        FCFEFDFBFEFCFBFEFCFAFDFBF9FDFBF7FDFAF6FDFAF5FDF9F4FCF8F2FCF8F1FC
        F7EFFCF6EEFDF8EFF7E6D69D897FC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D08F8F8FFFFFFFFCFCFCFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFCFC
        FCFCFCFCFBFBFBFBFBFBFAFAFAFAFAFAF9F9F9F8F8F8F7F7F7F6F6F6F6F6F6F5
        F5F5F4F4F4F5F5F5E3E3E3888888D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4A89086FFFFFFFEFEFDFEFEFEFEFEFEFFFFFFFEFEFEFEFEFEFEFE
        FEFEFDFDFEFDFCFEFCFBFEFCF9FDFBF8FDFBF7FDFAF6FDF9F4FDF9F3FCF8F1FC
        F7F0FCF7EFFDF7EFF6E6D89D8A7FC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D08F8F8FFFFFFFFDFDFDFEFEFEFEFEFEFFFFFFFEFEFEFEFEFEFEFE
        FEFDFDFDFCFCFCFBFBFBFBFBFBFAFAFAFAFAFAF9F9F9F7F7F7F7F7F7F6F6F6F5
        F5F5F5F5F5F5F5F5E3E3E3888888D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4A89186FFFFFFFEFEFEFFFFFFE0CFCEE0D0CFE0CFCDDFCDCADFCD
        CADFCBC8DFC9C5DFC8C2DFC7BDDFC5BBDFC3B8DFC1B4DFC0B1DFBFACDFBFACE1
        C2B0FCF7EFFDF7F0F6E6D89D8981C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0909090FFFFFFFEFEFEFFFFFFD0D0D0D1D1D1D0D0D0CECECECECE
        CECCCCCCCACACAC8C8C8C6C6C6C4C4C4C2C2C2C0C0C0BFBFBFBCBCBCBCBCBCC0
        C0C0F5F5F5F5F5F5E3E3E3888888D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4A99186FFFFFFFFFFFFFFFFFFDDCBC9DDCBCBDDCAC9DDC9C6DDC9
        C6DCC7C3DCC5C0DCC3BCDCC2B9DCC0B5DBBEB2DBBDAFDBBAAADBB9A7DBB9A7DE
        BEABFCF7EFFDF7F0F6E6D79E8980C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0909090FFFFFFFFFFFFFFFFFFCCCCCCCDCDCDCBCBCBCACACACACA
        CAC8C8C8C6C6C6C3C3C3C2C2C2BFBFBFBDBDBDBCBCBCB8B8B8B7B7B7B7B7B7BB
        BBBBF5F5F5F5F5F5E3E3E3888888D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4A99288FFFFFFFFFFFFFFFFFFDDCBC9DDCBCBDDCAC9DDC9C6DDC9
        C6DCC7C3DCC5C0DCC3BCDCC2B9DCC0B5DBBEB2DBBDAFDBBAAADBB9A7DBB9A7DE
        BEABFDF8F1FDF8F1F7E7D99E8A81C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0919191FFFFFFFFFFFFFFFFFFCCCCCCCDCDCDCBCBCBCACACACACA
        CAC8C8C8C6C6C6C3C3C3C2C2C2BFBFBFBDBDBDBCBCBCB8B8B8B7B7B7B7B7B7BB
        BBBBF6F6F6F6F6F6E4E4E4898989D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AA9387FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFEFEFDFEFDFCFEFCFAFFFDF9FEFDF9FFFDF9FFFDF7FEFBF7FD
        F8F3F9F5EDF5F0E8E9D6C89F8A81C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0929292FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFDFDFDFCFCFCFBFBFBFCFCFCFBFBFBFCFCFCFBFBFBFAFAFAF7
        F7F7F3F3F3EEEEEED3D3D3898989D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AB9388FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFEFEFEFEFDFCFEFCFAFFFDFBE6DCD7D9CBC2E0D3CCDACCC5D5
        C5BEC8B6ADBBA49A9775659E8A81C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0929292FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFEFEFEFCFCFCFBFBFBFCFCFCDBDBDBC9C9C9D2D2D2CBCBCBC4
        C4C4B5B5B5A3A3A3747474898989D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AB9388FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFEFEFDFEFDFCFEFCFAFFFFFFBBA295A18072A78978A5836FA3
        7F67B08866936C54917061A4938AC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0929292FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFDFDFDFCFCFCFBFBFBFFFFFFA0A0A07F7F7F8787878080807B
        7B7B8282826969696F6F6F929292D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AC9589FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFEFEFEFEFEFDFEFDFCFEFCFAFFFFFFCBB7ADC5AD9FFFF5E0F9E1BBF8
        D69CC59E6E906F61A89295C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0949494FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFEFEFEFDFDFDFCFCFCFBFBFBFFFFFFB6B6B6ABABABEFEFEFD8D8D8C8
        C8C89494946E6E6E959595D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AD9588FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFEFEFEFEFEFEFEFDFCFEFDFBFEFCFAFFFFFCCCBAB0BEA18BF6DFB8F3CF96C0
        986A8F6E62A7968CC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0939393FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFEFEFEFEFEFEFCFCFCFCFCFCFBFBFBFEFEFEB9B9B99D9D9DD5D5D5C2C2C28E
        8E8E6E6E6E949494D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AC9588FFFFFFFEFEFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFE
        FEFEFEFEFEFDFDFEFDFCFEFCFAFEFCF9FFFEFCD7C9C1B28D72F6D39AC89F6E90
        7063B19B95C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0939393FFFFFFFEFEFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFE
        FEFEFEFEFDFDFDFCFCFCFBFBFBFBFBFBFDFDFDC8C8C8898989C5C5C59595956F
        6F6F9B9B9BD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AD9489FFFFFFFEFEFDFEFEFEFEFEFEFFFFFFFEFEFEFEFEFEFEFE
        FEFEFDFDFEFDFCFEFCFBFEFCFAFDFBF8FFFFFBD3C2B9B7906ACBA16F917062AC
        958EC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0939393FFFFFFFDFDFDFEFEFEFEFEFEFFFFFFFEFEFEFEFEFEFEFE
        FEFDFDFDFCFCFCFBFBFBFBFBFBFAFAFAFDFDFDC1C1C18989899696966F6F6F95
        9595D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AB9488FFFFFFFEFDFCFEFDFDFEFEFDFEFEFDFEFEFDFEFEFDFEFD
        FCFEFDFCFEFCFBFEFCFAFDFBF9FDFBF7FFFEFBCEBAB1A2795A957464B19D92C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0939393FFFFFFFCFCFCFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFCFC
        FCFCFCFCFBFBFBFBFBFBFAFAFAFAFAFAFDFDFDB9B9B97474747272729B9B9BD0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AC9489FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB39789957263B39B97C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0939393FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF9696967171719C9C9CD0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4AF9D94AD958AAB9489AC9489AB9489AB9489AA9388A99388A993
        87A99188A89286A89187A79086A69086A69186A38E84AB9B90C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D09C9C9C9494949393939393939393939393939292929292929191
        919191919090909090908F8F8F8F8F8F9090908D8D8D999999D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      Margin = 2
      NumGlyphs = 2
    end
    object BtnCopia: TBitBtn
      Left = 108
      Top = 40
      Width = 102
      Height = 36
      Caption = '*Copia'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      OnClick = BtnCopiaClick
      OnMouseMove = AzzeraTimerMouseMove
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000EB0A0000EB0A00000000000000000000DC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00
        FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00
        FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        A3948D9E8A819D89809D89809D89809D89809C89809C89809C897F9C897F9C89
        7F9B897F9B887F9A887F9A877F9A877E9A877F99877E98867E99877F9E918ADC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        A0A0A0A0A0A09F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F9F
        9F9F9F9F9E9E9E9E9E9E9D9D9D9D9D9D9D9D9D9D9D9D9C9C9C9D9D9D9C9C9CDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        9D8A81F8E4D3F2DECCF2DECCF2DECCF2DECCF2DECCF2DFCCF2DDCCF2DDCBF2DD
        CBF2DECAF2DCCAF2DCC7F3DBC7F1DAC4F1D9C2F2D8C0F1D6BDF5D9BD98867EDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        A0A0A0EDEDEDEAEAEAEAEAEAEAEAEAEAEAEAEAEAEAEAEAEAE9E9E9E9E9E9E9E9
        E9E9E9E9E8E8E8E8E8E8E8E8E8E7E7E7E6E6E6E5E5E5E4E4E4E6E6E69C9C9CDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        9E8A81FFFDF2FAF3E9FAF3E9FAF3E9FAF3E9FAF3E8FAF4E8FAF2E8FAF2E7FAF2
        E7FAF3E6FAF1E6FAF2E5FAF2E4F9F1E2F9F0E0F9EFDEF9EDD9F4DCC497857CDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        A0A0A0FCFCFCF6F6F6F6F6F6F6F6F6F6F6F6F6F6F6F7F7F7F6F6F6F6F6F6F6F6
        F6F6F6F6F5F5F5F6F6F6F6F6F6F4F4F4F4F4F4F3F3F3F2F2F2E8E8E89B9B9BDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        9F8B81FFFEF7FCF6ECDEC4B7DEC5B7DEC5B7DDC3B5DDC3B5DDC2B3DDC1B2DDC1
        B0DDBFAEDDBEABDDBDAADDBCA8DDBAA5DDBAA5DFBEA9FAF0E1F3DFC998867DDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        A1A1A1FDFDFDF8F8F8DBDBDBDBDBDBDBDBDBDADADADADADAD9D9D9D9D9D9D9D9
        D9D8D8D8D7D7D7D6D6D6D6D6D6D5D5D5D5D5D5D7D7D7F4F4F4EAEAEA9C9C9CDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        9F8A81FFFFF7FCF6EEDBC0B4DBC0B4DBC0B4DBC0B3DBBFB2DBBEB1DBBDAEDABB
        ACDABAAADAB9A8DAB7A5DAB6A2DAB5A0DAB5A0DDBAA4FBF3E4F5E0CC85695A98
        74829873819873819773809673809774819C818CDC00FFDC00FFDC00FFDC00FF
        A0A0A0FDFDFDF9F9F9D9D9D9D9D9D9D9D9D9D9D9D9D8D8D8D8D8D8D7D7D7D6D6
        D6D5D5D5D4D4D4D3D3D3D2D2D2D2D2D2D2D2D2D5D5D5F6F6F6EBEBEB9C9C9C91
        91919090909090909090908F8F8F9090908F8F8FDC00FFDC00FFDC00FFDC00FF
        A08B82FFFFFAFCF7EFDBC2B8DBC2B8DBC2B8DBC2B6DBC0B5DBBFB2DBBEB1DBBD
        AFDBBCACDABAAADAB9A6DAB7A4DAB6A1DAB6A1DDBBA5FBF4E7F4E2CE7E5B4CDF
        B0B9DDAEB4DDACB4DEAAB3DDA7B0E1A7AF967380DC00FFDC00FFDC00FFDC00FF
        A1A1A1FEFEFEF9F9F9DADADADADADADADADADADADAD9D9D9D8D8D8D8D8D8D7D7
        D7D6D6D6D5D5D5D4D4D4D3D3D3D2D2D2D2D2D2D5D5D5F7F7F7EBEBEB9C9C9CD3
        D3D3D1D1D1D0D0D0D0D0D0CECECECECECE8F8F8FDC00FFDC00FFDC00FFDC00FF
        A18C83FFFFFAFCF8F1DBC3BBDBC4BBDBC4BBDBC3B9DBC2B8DBC0B6DBBFB3DBBE
        B1DBBDAEDBBCACDAB9A8DAB8A5DAB7A3DAB7A3DDBBA7FCF5E8F6E3D2805D4BE3
        CCCCE2CCCBE2C9C9E3C6C6E3C1C1E0ADB495717FDC00FFDC00FFDC00FFDC00FF
        A2A2A2FEFEFEF9F9F9DBDBDBDBDBDBDBDBDBDBDBDBDADADAD9D9D9D8D8D8D8D8
        D8D7D7D7D6D6D6D4D4D4D3D3D3D3D3D3D3D3D3D5D5D5F7F7F7EDEDED9D9D9DE1
        E1E1E1E1E1DFDFDFDEDEDEDBDBDBD1D1D18E8E8EDC00FFDC00FFDC00FFDC00FF
        A18C83FFFFFBFCF8F2DBC6BEDBC6BFDBC6BFDBC4BCDBC3BADBC1B7DBC0B5DBBE
        B3DBBDAFDBBCACDBBBAADBBAA8DAB7A4DAB7A4DDBDA8FCF4E9F5E4D37F5C4DCE
        92A2CE8F9FCE8F9FCF93A2E3CACBDFB2B795727FDC00FFDC00FFDC00FFDC00FF
        A2A2A2FEFEFEFAFAFADCDCDCDCDCDCDCDCDCDBDBDBDBDBDBD9D9D9D9D9D9D8D8
        D8D7D7D7D6D6D6D5D5D5D5D5D5D3D3D3D3D3D3D6D6D6F7F7F7EDEDED9D9D9DC3
        C3C3C1C1C1C1C1C1C3C3C3E0E0E0D3D3D38E8E8EDC00FFDC00FFDC00FFDC00FF
        A28E83FFFFFEFDF9F4DBC7C2DBC7C3DBC7C3DBC5BFDBC4BCDBC3BADBC1B7DBBF
        B3DBBEB1DBBCAFDBBBABDBBAA8DBB8A5DBB8A5DDBDAAFCF6EAF5E4D2805E4CCB
        8C9ECB8A9CCB8A9CCE8F9FE4CECDE1B5BC967380DC00FFDC00FFDC00FFDC00FF
        A3A3A3FEFEFEFAFAFADEDEDEDEDEDEDEDEDEDCDCDCDBDBDBDBDBDBD9D9D9D8D8
        D8D8D8D8D6D6D6D5D5D5D5D5D5D3D3D3D3D3D3D6D6D6F8F8F8EDEDED9D9D9DBF
        BFBFBEBEBEBEBEBEC1C1C1E2E2E2D5D5D58F8F8FDC00FFDC00FFDC00FFDC00FF
        A48D84FFFFFFFDFAF5DBC8C3DBC8C4DBC8C4DBC6C0DBC4BDDBC3BADBC2B8DBC0
        B5DBBEB2DBBDAFDBBBACDBBAA9DBB9A7DBB9A7DEBEAAFCF5EBF5E4D3825D4CCB
        8E9FCB8C9CCB8C9CCE8F9DE4CFCFDFB7BC977381DC00FFDC00FFDC00FFDC00FF
        A3A3A3FFFFFFFBFBFBDEDEDEDEDEDEDEDEDEDCDCDCDBDBDBDBDBDBDADADAD9D9
        D9D8D8D8D7D7D7D6D6D6D5D5D5D4D4D4D4D4D4D7D7D7F7F7F7EDEDED9D9D9DC0
        C0C0BFBFBFBFBFBFC1C1C1E3E3E3D6D6D6909090DC00FFDC00FFDC00FFDC00FF
        A48E83FFFFFFFDFBF7DBC7C3DBC7C4DBC7C4DBC6C0DBC5BEDBC4BBDBC2B8DBC0
        B5DBBFB3DBBDAFDBBCACDBBAAADBB8A6DBB8A6DEBEABFCF5ECF5E4D4815F4ECB
        90A1CB8E9FCB8E9FCE90A1E5D0CFE1BBC1987381DC00FFDC00FFDC00FFDC00FF
        A3A3A3FFFFFFFCFCFCDEDEDEDEDEDEDEDEDEDCDCDCDCDCDCDBDBDBDADADAD9D9
        D9D8D8D8D7D7D7D6D6D6D5D5D5D3D3D3D3D3D3D7D7D7F7F7F7EDEDED9F9F9FC1
        C1C1C0C0C0C0C0C0C1C1C1E3E3E3D9D9D9909090DC00FFDC00FFDC00FFDC00FF
        A68F85FFFFFFFEFCFADCC6C0DCC6C0DCC6C0DCC5BFDCC4BDDBC3BADBC2B7DBC0
        B5DBBEB2DBBDAFDBBCADDBBAAADBB9A7DBB9A7DEBDABFCF6EDF5E5D583604DCC
        92A2CB8E9FCB8E9FCE92A1E5D1D2E0BCC1987381DC00FFDC00FFDC00FFDC00FF
        A5A5A5FFFFFFFCFCFCDDDDDDDDDDDDDDDDDDDCDCDCDCDCDCDBDBDBDADADAD9D9
        D9D8D8D8D7D7D7D6D6D6D5D5D5D4D4D4D4D4D4D6D6D6F8F8F8EEEEEE9F9F9FC3
        C3C3C0C0C0C0C0C0C3C3C3E4E4E4D9D9D9909090DC00FFDC00FFDC00FFDC00FF
        A69084FFFFFFFEFDFBDCC6C0DCC6C0DCC6C0DCC4BEDCC4BBDCC3BADBC1B7DBC0
        B4DBBEB2DBBDAFDBBCADDBBBAADBB9A7DBB9A7DEBDAAFCF6EEF5E5D7845F4FCC
        92A2CC8FA0CC8FA0CE94A3E5D2D1E0BCC098737FDC00FFDC00FFDC00FFDC00FF
        A5A5A5FFFFFFFDFDFDDDDDDDDDDDDDDDDDDDDCDCDCDBDBDBDBDBDBD9D9D9D9D9
        D9D8D8D8D7D7D7D6D6D6D5D5D5D4D4D4D4D4D4D6D6D6F9F9F9EEEEEE9F9F9FC3
        C3C3C1C1C1C1C1C1C3C3C3E4E4E4D9D9D9909090DC00FFDC00FFDC00FFDC00FF
        A79185FFFFFFFEFDFDFEFEFDFEFEFDFEFEFDFEFDFCFEFDFBFEFCFBFEFCFAFDFB
        F9FDFBF7FDFAF6FDFAF5FDF9F4FCF8F2FCF8F1FCF7EFFDF8EFF7E6D6845F4ECC
        92A4CC90A2CC90A2CE94A3E5D3D4E0BCC1997381DC00FFDC00FFDC00FFDC00FF
        A6A6A6FFFFFFFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFCFCFCFCFCFCFCFC
        FCFCFCFCFBFBFBFBFBFBFAFAFAFAFAFAF9F9F9F9F9F9F9F9F9EEEEEE9F9F9FC3
        C3C3C1C1C1C1C1C1C3C3C3E5E5E5D9D9D9909090DC00FFDC00FFDC00FFDC00FF
        A89186FFFFFFFFFFFFE0CFCEE0D0CFE0CFCDDFCDCADFCBC8DFC9C5DFC8C2DFC7
        BDDFC5BBDFC3B8DFC1B4DFC0B1DFBFACDFBFACE1C2B0FDF7F0F6E6D8856050CC
        94A5CC90A2CC90A2CE96A4E5D3D4E0BDC2997581DC00FFDC00FFDC00FFDC00FF
        A6A6A6FFFFFFFFFFFFE2E2E2E3E3E3E2E2E2E1E1E1E0E0E0DFDFDFDEDEDEDDDD
        DDDCDCDCDBDBDBD9D9D9D9D9D9D8D8D8D8D8D8D9D9D9F9F9F9EEEEEE9F9F9FC4
        C4C4C1C1C1C1C1C1C4C4C4E5E5E5D9D9D9919191DC00FFDC00FFDC00FFDC00FF
        A99186FFFFFFFFFFFFDDCBC9DDCBCBDDCAC9DDC9C6DCC7C3DCC5C0DCC3BCDCC2
        B9DCC0B5DBBEB2DBBDAFDBBAAADBB9A7DBB9A7DEBEABFDF7F0F6E6D786604FCC
        94A5CC90A2CC90A2CE96A6E5D5D6E0BDC29A7381DC00FFDC00FFDC00FFDC00FF
        A7A7A7FFFFFFFFFFFFE0E0E0E0E0E0DFDFDFDFDFDFDEDEDEDCDCDCDBDBDBDADA
        DAD9D9D9D8D8D8D7D7D7D5D5D5D4D4D4D4D4D4D7D7D7F9F9F9EEEEEE9F9F9FC4
        C4C4C1C1C1C1C1C1C4C4C4E6E6E6D9D9D9909090DC00FFDC00FFDC00FFDC00FF
        A99288FFFFFFFFFFFFDDCBC9DDCBCBDDCAC9DDC9C6DCC7C3DCC5C0DCC3BCDCC2
        B9DCC0B5DBBEB2DBBDAFDBBAAADBB9A7D5B3A0D3B29EE6DACFDBC4B4866150CC
        94A4CC90A2CC90A2CE94A4E4D6D7E0BFC59A7584DC00FFDC00FFDC00FFDC00FF
        A8A8A8FFFFFFFFFFFFE0E0E0E0E0E0DFDFDFDFDFDFDEDEDEDCDCDCDBDBDBDADA
        DAD9D9D9D8D8D8D7D7D7D5D5D5D4D4D4D0D0D0D0D0D0E8E8E8DBDBDBA0A0A0C3
        C3C3C1C1C1C1C1C1C4C4C4E7E7E7DBDBDB919191DC00FFDC00FFDC00FFDC00FF
        AB9388FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEFEFC
        FAFFFDFBE6DCD7D9CBC2E0D3CCDACCC5D5C5BEC8B6ADBBA49A977565A18579E5
        DCDDE4D9DAE4D9D9E5D7D7E5D7D7E1BFC49A7483DC00FFDC00FFDC00FFDC00FF
        A8A8A8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEFCFC
        FCFDFDFDEAEAEADFDFDFE4E4E4E0E0E0DCDCDCD3D3D3C8C8C8ACACACB5B5B5EA
        EAEAE8E8E8E8E8E8E7E7E7E7E7E7DBDBDB919191DC00FFDC00FFDC00FFDC00FF
        AB9388FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFDFEFC
        FAFFFFFFBBA295A18072A78978A5836FA37F67B08866936C549A7269CB9BA9CF
        9AABCF97A5CF97A5D09AA9E5D8D9E1C0C69B7584DC00FFDC00FFDC00FFDC00FF
        A9A9A9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDFDFDFCFC
        FCFFFFFFC6C6C6B2B2B2B7B7B7B4B4B4B1B1B1B5B5B5A6A6A6ABABABC7C7C7C7
        C7C7C5C5C5C5C5C5C7C7C7E8E8E8DBDBDB929292DC00FFDC00FFDC00FFDC00FF
        AC9589FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEFEFEFDFEFC
        FAFFFFFFCBB7ADC5AD9FFFF5E0F9E1BBF8D69CC59E6E997269C899A8CB99A9CC
        94A5CC90A2CC90A2CE96A4E5D8D9E1C0C59C7584DC00FFDC00FFDC00FFDC00FF
        A9A9A9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEFDFDFDFCFC
        FCFFFFFFD3D3D3CDCDCDF6F6F6E9E9E9E1E1E1C1C1C1ABABABC6C6C6C6C6C6C4
        C4C4C1C1C1C1C1C1C4C4C4E8E8E8DBDBDB929292DC00FFDC00FFDC00FFDC00FF
        AD9588FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEFEFEFEFEFDFCFEFC
        FAFFFFFCCCBAB0BEA18BF6DFB8F3CF96C0986A956E66C89CAACB9CACCB99A9CC
        94A5CC90A2C78A9BC68899D3B8BDCB9EA99C7584DC00FFDC00FFDC00FFDC00FF
        AAAAAAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEFEFEFEFDFDFDFCFC
        FCFEFEFED5D5D5C5C5C5E8E8E8DDDDDDBDBDBDA9A9A9C7C7C7C7C7C7C6C6C6C4
        C4C4C1C1C1BEBEBEBDBDBDD6D6D6C8C8C8929292DC00FFDC00FFDC00FFDC00FF
        AC9588FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEFEFEFEFEFDFDFEFDFCFEFC
        F9FFFEFCD7C9C1B28D72F6D39AC89F6E9B7E72E1D9D8E6E2E3D3BFC6C9ABB2CE
        B4BCC9AEB6C6A7B1BB97A2B184929351659C7584DC00FFDC00FFDC00FFDC00FF
        A9A9A9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFEFEFEFEFEFDFDFDFDFDFDFCFC
        FCFDFDFDDEDEDEB9B9B9DFDFDFC1C1C1B1B1B1E8E8E8EDEDEDDADADACFCFCFD4
        D4D4D0D0D0CDCDCDC3C3C3B9B9B99C9C9CB1B1B1DC00FFDC00FFDC00FFDC00FF
        AD9489FFFFFFFEFEFEFEFEFEFFFFFFFEFEFEFEFEFEFEFDFDFEFDFCFEFCFBFDFB
        F8FFFFFBD3C2B9B7906ACBA16F9D7D71E1DCDAE6E1E1E6E6E6B17E8E9C5D72A2
        6375A05A6C9F5163AA51629042568E5266A2818EDC00FFDC00FFDC00FFDC00FF
        A9A9A9FFFFFFFEFEFEFEFEFEFFFFFFFEFEFEFEFEFEFDFDFDFDFDFDFCFCFCFCFC
        FCFEFEFED9D9D9BABABAC3C3C3B1B1B1EAEAEAEDEDEDF0F0F0B6B6B6A3A3A3A6
        A6A6A2A2A29D9D9D9D9D9D949494ADADADDC00FFDC00FFDC00FFDC00FFDC00FF
        AB9488FFFFFFFEFDFDFEFEFDFEFEFDFEFEFDFEFDFCFEFDFCFEFCFBFEFCFAFDFB
        F7FFFEFBCEBAB1A2795A9D7D6DDDDAD8E6E5E4E6E1E1E6E6E6BD96A2B98895E6
        CCCBE4A5A7E58787BC5A638C5266A589A1DC00FFDC00FFDC00FFDC00FFDC00FF
        A9A9A9FFFFFFFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFCFCFCFCFCFCFCFC
        FCFDFDFDD5D5D5ACACACB1B1B1E8E8E8EEEEEEEDEDEDF0F0F0C3C3C3BBBBBBE1
        E1E1CCCCCCBCBCBCA2A2A2AEAEAEDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        AC9489FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFB397899E7C6DDEDADAE4E5E6E6E2E3E6E1E1E6E5E4BE99A3B47581E1
        A2A2E18385B7565F8B5368A5828EDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        A9A9A9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFC0C0C0B0B0B0E8E8E8EFEFEFEDEDEDEDEDEDEEEEEEC4C4C4B1B1B1CA
        CACABABABAA0A0A0A8A8A8DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        AF9D94AD958AAC9489AB9489AB9489AA9388A993879C7868966E59956C5A936A
        58936C59A68A7DDDDBD9E4E5E6E6E4E5E6E2E3E6E1E1E6E4E4C6AAB2AB5D6CE3
        8687BD5A638D5367AE899ADC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        A9A9A9AAAAAAA9A9A9A9A9A9A9A9A9A8A8A8A8A8A8A7A7A7A7A7A7A6A6A6A5A5
        A5A6A6A6B8B8B8E8E8E8EFEFEFEEEEEEEDEDEDEDEDEDEEEEEECECECEA3A3A3BB
        BBBBA2A2A2AAAAAADC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFAA7B8DE6E6E6E4E5E6E4E5
        E6E6E6E6E4E5E6E4E5E6E6E4E5E6E2E3E6E2E3E5E0E0E6E3E2C4A2ABB05663C0
        5B658E5368AA8396DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF999999F0F0F0EFEFEFEFEF
        EFF0F0F0EFEFEFEFEFEFEEEEEEEDEDEDEDEDEDECECECEEEEEECACACAA0A0A0A3
        A3A3A9A9A9DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFA87A89E6E6E6E6E4E5E6E5
        E4E6E5E4E6E5E4E6E2E3E6E2E3E6E2E3E6E1E1E5DEDEE6E2E2C09AA79D475791
        5467AE8694DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF989898F0F0F0EEEEEEEEEE
        EEEEEEEEEEEEEEEDEDEDEDEDEDEDEDEDEDEDEDEBEBEBEDEDEDC6C6C6979797A7
        A7A7DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFAA7B8DE6E6E6E6E6E6E6E6
        E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6E6AB7384915367B1
        8AA0DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF999999F0F0F0F0F0F0F0F0
        F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0B0B0B0A6A6A6DC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFAD8897AA7C8DAA7B8DA87B
        8CA87B8CA87A8AA67A8AA67A8BA67A88A57A8BA47989A47A88A17888A98490DC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF9999999A9A9A9999999999
        99999999989898989898989898979797989898979797979797B2B2B2DC00FFDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00
        FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF
        DC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00
        FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC
        00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FFDC00FF}
      Margin = 2
      NumGlyphs = 2
    end
    object BtnStampa: TBitBtn
      Left = 108
      Top = 75
      Width = 102
      Height = 36
      Caption = '*Stampa'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      OnClick = BtnStampaClick
      OnMouseMove = AzzeraTimerMouseMove
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000F00A0000F00A00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C6CDD1A9AD
        AFA9ADB0C3CBCDC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0CDCDCDADAD
        ADADADADCACACAD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4B2B7BA98999AA2A2
        A2999999959697A4A8AABABFC2C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0B7B7B7999999A2A2
        A2999999969696A8A8A8BFBFBFD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4B6BABDADB1B3ADAEAFAAAAAA8D8D
        8D7D7D7D939393919191969798A6AAACA4A8AAC3CACDC8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0BABABAB1B1B1AEAEAEAAAAAA8D8D
        8D7D7D7D939393919191979797AAAAAAA8A8A8CACACAD0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4B8BEC1AFB3B5B3B4B5B2B2B28E8E8E6161617B7B
        7B8484846363636565654444445959597D7D7D919293A2A6A8B8BDC0C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0BEBEBEB3B3B3B4B4B4B2B2B28E8E8E6161617B7B
        7B8484846363636565654444445959597D7D7D929292A6A6A6BDBDBDD0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4BCC1C4B1B5B7B7B9B9B7B7B7909090666666868686C1C1C1DBDB
        DBB3B3B3ADADAD9393935C5C5C2B2B2B313131424242646464898B8BA2A6A8A0
        A5A7C2C9CCC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0C1C1C1B5B5B5B8B8B8B7B7B7909090666666868686C1C1C1DBDB
        DBB3B3B3ADADAD9393935C5C5C2B2B2B3131314242426464648A8A8AA6A6A6A5
        A5A5C9C9C9D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C3C9CCB2B6B8BCBDBDBDBDBD9393936E6E6E979797CFCFCFDDDDDDD6D6D6D4D4
        D4B1B1B1ACACACB2B2B27575754C4C4C2D2D2D2B2B2B2E2E2E3131314C4C4C6A
        6A6A8586879DA1A3B6BCBEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        C9C9C9B6B6B6BCBCBCBDBDBD9393936E6E6E979797CFCFCFDDDDDDD6D6D6D4D4
        D4B1B1B1ACACACB2B2B27575754C4C4C2D2D2D2B2B2B2E2E2E3131314C4C4C6A
        6A6A868686A1A1A1BCBCBCD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        B0B5B7C2C2C2969696787878A7A7A7DBDBDBE0E0E0DCDCDCD7D7D7D5D5D5D6D6
        D6B1B1B1B0B0B0B1B1B1B1B1B18B8B8B6B6B6B5151513C3C3C2A2A2A2929292C
        2C2C3737375353537A7B7C9CA0A29DA1A3C6CED0C8D0D4C8D0D4D0D0D0D0D0D0
        B5B5B5C2C2C2969696787878A7A7A7DBDBDBE0E0E0DCDCDCD7D7D7D5D5D5D6D6
        D6B1B1B1B0B0B0B1B1B1B1B1B18B8B8B6B6B6B5151513C3C3C2A2A2A2929292C
        2C2C3737375353537B7B7BA0A0A0A1A1A1CDCDCDD0D0D0D0D0D0C8D0D4C8D0D4
        A8ACAE878787B7B7B7E4E4E4E5E5E5E0E0E0DCDCDCDBDBDBD9D9D9D7D7D7D7D7
        D7B2B2B2B3B3B3B2B2B2B3B3B3B4B5B5B6B7B7A0A1A18283836767675050503C
        3C3C2929292929292C2C2C3C3C3C656565A7ABAEC8D0D4C8D0D4D0D0D0D0D0D0
        ACACAC878787B7B7B7E4E4E4E5E5E5E0E0E0DCDCDCDBDBDBD9D9D9D7D7D7D7D7
        D7B2B2B2B3B3B3B2B2B2B3B3B3B4B4B4B6B6B6A0A0A08282826767675050503C
        3C3C2929292929292C2C2C3C3C3C656565ABABABD0D0D0D0D0D0C8D0D4B8BDC0
        C6C7C8EBEBEBE9E9E9E4E4E4E1E1E1E0E0E0DEDEDEDDDDDDDBDBDBD9D9D9D3D3
        D3B7B7B7B7B7B7B6B6B6B6B7B7B5B8BBB4B8BEB5B6BAB7BBC1B6BDC59DA3A781
        83846768684F4F4F39393930303066666695999BC8D0D4C8D0D4D0D0D0BDBDBD
        C7C7C7EBEBEBE9E9E9E4E4E4E1E1E1E0E0E0DEDEDEDDDDDDDBDBDBD9D9D9D3D3
        D3B7B7B7B7B7B7B6B6B6B6B6B6B8B8B8B9B9B9B7B7B7BCBCBCBEBEBEA3A3A383
        83836767674F4F4F393939303030666666999999D0D0D0D0D0D0C8D0D4B3B7BA
        EBEBEBE9E9E9E6E6E6E5E5E5E4E4E4E2E2E2E1E1E1DFDFDFDDDDDDDDDDDDCFCF
        CFBABABABBBBBBBABABAB9BCBFC2A98CD67B14D77B11D4740ACC7A1FC69059BB
        B4ADB8BDC39F9FA08080806F6F6F9E9E9EA9AAABACB2B5C8D0D4D0D0D0B7B7B7
        EBEBEBE9E9E9E6E6E6E5E5E5E4E4E4E2E2E2E1E1E1DFDFDFDDDDDDDDDDDDCFCF
        CFBABABABBBBBBBABABABCBCBCA3A3A36666666565655F5F5F686868858585B2
        B2B2BEBEBE9F9F9F8080806F6F6F9E9E9EAAAAAAB2B2B2D0D0D0C8D0D4B9BDBF
        E6E6E6ECECECE9E9E9E7E7E7E6E6E6E4E4E4E3E3E3E1E1E1DFDFDFDEDEDECACA
        CABFBFBFBEBEBEBEBEC0BEC0C3D5811EE8AC5FE9A95AE7A858E39F47DB8A29D1
        6A00C29465B8BEC3BBBBBBBCBCBCB9B9B9B3B3B3A5A9ACC8D0D4D0D0D0BDBDBD
        E6E6E6ECECECE9E9E9E7E7E7E6E6E6E4E4E4E3E3E3E1E1E1DFDFDFDEDEDECACA
        CABFBFBFBEBEBEBEBEBEC0C0C06C6C6C9B9B9B9898989797978C8C8C76767656
        56568B8B8BBEBEBEBBBBBBBCBCBCB9B9B9B3B3B3A9A9A9D0D0D0C8D0D4C1C6C9
        DDDDDDEEEEEEEBEBEBEAEAEAE8E8E8E6E6E6E4E4E4E2E2E2E0E0E0E0E0E0C5C5
        C5C3C3C3C2C2C2C2C5CBCCA574E29A43EBB46DE7A858E6A655E4A454E4A651E4
        A34ED36D00BEB2A6BCBDBDC0BDBABCBBBAAAAAAAB1B6B9C8D0D4D0D0D0C6C6C6
        DDDDDDEEEEEEEBEBEBEAEAEAE8E8E8E6E6E6E4E4E4E2E2E2E0E0E0E0E0E0C5C5
        C5C3C3C3C2C2C2C6C6C69A9A9A888888A5A5A597979795959593939393939390
        9090585858AFAFAFBCBCBCBCBCBCBABABAAAAAAAB6B6B6D0D0D0C8D0D4C7CFD2
        CED0D0F3F3F3EEEEEEECECECEAEAEAE8E8E8E6E6E6E4E4E4E4E4E4D9D9D9C7C7
        C7C7C7C7C6C7C8C7C7C9D78521EEBB7CEBAE64E8AA5BE6A858E5A654E4A450E5
        A552D77D17C6AD92B1B7BD79B6EECBC7C5A0A2A1C3CBCEC8D0D4D0D0D0CFCFCF
        CFCFCFF3F3F3EEEEEEECECECEAEAEAE8E8E8E6E6E6E4E4E4E4E4E4D9D9D9C7C7
        C7C7C7C7C7C7C7C7C7C7707070ADADAD9E9E9E99999997979794949492929293
        9393686868A7A7A7B8B8B8BFBFBFC6C6C6A1A1A1CACACAD0D0D0C8D0D4C8D0D4
        C1C5C7F7F7F7F0F0F0EEEEEEECECECEAEAEAE8E8E8E5E5E5E5E5E5D0D0D0CBCB
        CBCACACACACED4D2A771E4A14EEFBD7EEAAD62E9AB5EE7A95AE6A756E5A553E4
        A654D77A11C0AD9C5BA3DA36AEFFB8C4CDA6AAABC8D0D4C8D0D4D0D0D0D0D0D0
        C5C5C5F7F7F7F0F0F0EEEEEEECECECEAEAEAE8E8E8E5E5E5E5E5E5D0D0D0CBCB
        CBCACACACFCFCF9B9B9B8F8F8FAFAFAF9D9D9D9B9B9B98989895959593939394
        9494656565AAAAAAABABABB8B8B8C5C5C5A9A9A9D0D0D0D0D0D0C8D0D4C8D0D4
        BCC0C2F5F5F5F3F3F3F0F0F0EEEEEEEBEBEBE9E9E9E7E7E7E0E0E0D2D2D2D1D1
        D1D0D2D5D1C9C0DC8E2AF1C085EDB570EBAE64E9AC60E8AA5CE7A858E5A656E5
        A552D57711AFAEB14CA5E151B4FAC4BEBA9EA2A4C8D0D4C8D0D4D0D0D0D0D0D0
        C0C0C0F5F5F5F3F3F3F0F0F0EEEEEEEBEBEBE9E9E9E7E7E7E0E0E0D2D2D2D1D1
        D1D2D2D2C7C7C7787878B3B3B3A6A6A69E9E9E9C9C9C99999997979795959593
        9393636363AFAFAFACACACBDBDBDBDBDBDA2A2A2D0D0D0D0D0D0C8D0D4C8D0D4
        C9D0D3CDCFCFFAFAFAF3F3F3EFEFEFECECECEBEBEBE7E7E7DADADAD8D8D8D7D8
        DAD6D8DDDA9842EEB470F2BF82EDB26BECB066EAAE62E9AB5EE7A95AE6A858E0
        983DD19652CDD1D67E8A91A8AAADACACACC4CACEC8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0CECECEFAFAFAF3F3F3EFEFEFECECECEBEBEBE7E7E7DADADAD8D8D8D8D8
        D8D9D9D9858585A6A6A6B2B2B2A3A3A3A0A0A09E9E9E9B9B9B98989897979784
        8484888888D2D2D28A8A8AAAAAAAACACACCACACAD0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C3C8CBE3E4E4F9F9F9F3F3F3F0F0F0EBEBEBE1E1E1DFDFDFDEDFE1DDE2
        E8DEA75FE8AC5DF4C58EF0B874EEB36CECB168EBAF64E9AC60E8AA5DE9AB5DD9
        821BD1C0ADD0D2D5D5D4D3CFD0D0AEB4B6C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0C8C8C8E3E3E3F9F9F9F3F3F3F0F0F0EBEBEBE1E1E1DFDFDFDFDFDFE3E3
        E39797979B9B9BB9B9B9AAAAAAA4A4A4A1A1A19F9F9F9C9C9C9A9A9A9A9A9A6D
        6D6DBCBCBCD2D2D2D3D3D3CFCFCFB3B3B3D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C3C9CCCBCCCDD6D6D7D8D8D9DDDEDFE4E5E7E7EAEEE4E3E3E4AE
        67EAAE63F7CA97F2BC7DF0B672EEB46EEDB26AEBAF65EAAD61EAAD5FE39E47D7
        9444D6DDE5D7D8D8DADADAADB1B3C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0C9C9C9CCCCCCD6D6D6D8D8D8DEDEDEE5E5E5EAEAEAE3E3E39E9E
        9E9E9E9EBFBFBFAFAFAFA8A8A8A5A5A5A3A3A39F9F9F9D9D9D9C9C9C8B8B8B83
        8383DEDEDED7D7D7DADADAB1B1B1D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C7CDD0B4B3AFCAC5BFBDB5ACBEAC99D3B28AECAF63F4C0
        83FACD9EF4BF82F2B977F0B773EFB56FEDB26BECB067EBAF66EAAB5BDA8824DF
        D8D1E2E4E5D8D9D9B1B5B7C6CDD1C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0CDCDCDB1B1B1C3C3C3B3B3B3A8A8A8A9A9A99F9F9FB3B3
        B3C4C4C4B2B2B2ABABABA9A9A9A6A6A6A3A3A3A1A1A1A0A0A09A9A9A737373D6
        D6D6E4E4E4D8D8D8B5B5B5CDCDCDD0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C3C9CCD9D1C6FFFAE6FFEED3FABD78F4B770F5C48CF6C2
        89F5BC7CF4BB7BF3BC7BF3BB78F0B774F0B670EEB46EE4B06FD3B080C4B198C7
        CCD0BCC0C2B6BBBEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0C9C9C9CECECEF4F4F4E7E7E7AFAFAFA8A8A8B8B8B8B6B6
        B6AFAFAFAEAEAEAEAEAEADADADA9A9A9A7A7A7A5A5A5A2A2A2A5A5A5ABABABCC
        CCCCC0C0C0BBBBBBD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4B8BDC0F9EFDFFFF5E5FFF4E4FFF1DDFBDEBBF7CF9EF3C2
        86EDB66EE8AA59E3A14AE09C40E19D3EE0A454D2B589CFC6B1C8D0D4C8CFD3C7
        CED2C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0BDBDBDEBEBEBF1F1F1F0F0F0ECECECD6D6D6C4C4C4B5B5
        B5A6A6A69898988E8E8E888888888888929292ABABABC0C0C0D0D0D0CFCFCFCE
        CECED0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4B0B4B7FFFEF1FFF6E6FFF4E4FFF4E3FFF3E1FFF3E1FFF3
        DEFFF2DDFFF2DAFFF0D7FFEFD0CCC3B0C5CBCEC8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0B4B4B4FAFAFAF2F2F2F0F0F0F0F0F0EEEEEEEEEEEEEEEE
        EEEDEDEDECECECEAEAEAE7E7E7BEBEBECBCBCBD0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4BFC2C2FFFDF2FFF6EBFFF5E7FFF4E4FFF2E0FFF1DDFFEF
        D9FFEED5FFEDD2FFEBCFFFF1D0B6B7B6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0C1C1C1F9F9F9F3F3F3F1F1F1F0F0F0EEEEEEECECECEAEA
        EAE8E8E8E6E6E6E4E4E4E8E8E8B6B6B6D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C7CFD2D3D1CEFFFCF2FFF7ECFFF6E9FFF4E6FFF3E2FFF2DEFFF0
        DBFFEFD7FFEDD3FFEDCFFFF1CFA7ACB0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0CECECED0D0D0F9F9F9F4F4F4F3F3F3F1F1F1EFEFEFEDEDEDEBEB
        EBE9E9E9E7E7E7E6E6E6E8E8E8ACACACD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4BEC4C5F9F6F0FFFFFBFFFEF5FFFDF0FFFAEBFFF9E6FFF5E3FFF3
        DFFFF2DAFFEFD6FFF0D3DFD3C0C0C7CBC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0C3C3C3F4F4F4FDFDFDFBFBFBF9F9F9F6F6F6F4F4F4F0F0F0EEEE
        EEECECECE9E9E9E9E9E9CECECEC7C7C7D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4B6BCBEBCC0C1BFC2C3C1C5C6C5C7C7C8CAC8CCCCCAD5CFC6DFD6
        C7E8DBC8EFE1CAFCEBCFC1C1BCC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0BBBBBBBFBFBFC1C1C1C4C4C4C6C6C6C9C9C9CBCBCBCCCCCCD2D2
        D2D6D6D6DBDBDBE4E4E4BFBFBFD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C6CED2C2C7
        CABEC4C6B9BEC1B5BABDB9BEC2C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0CECECEC7C7
        C7C4C4C4BEBEBEBABABABEBEBED0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      Margin = 2
      NumGlyphs = 2
    end
    object BtnInserimento: TBitBtn
      Left = 108
      Top = 110
      Width = 102
      Height = 36
      Caption = '*Inserimento'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
      OnClick = BtnInserimentoClick
      OnMouseMove = AzzeraTimerMouseMove
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFFCFCFCFBFCFCFCFCFDFCFD
        FDFCFDFDFCFCFDFCFCFCFCFCFDFCFCFDFCFCFCFCFCFDFCFDFDFCFCFDFCFCFCFC
        FCFDFCFDFDFCFDFDFCFCFDFBFBFCFCFCFCFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFFCFCFCFBFBFBFCFCFCFCFC
        FCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFCFC
        FCFCFCFCFCFCFCFCFCFCFCFBFBFBFCFCFCFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFFAFAFAFBFBFCFDFFFFFEFF
        FFFDFFFFFCFEFFFCFDFEFDFFFFFCFEFFFCFDFEFDFFFFFDFFFFFCFEFFFCFDFEFD
        FFFFFEFFFFFDFFFFFCFEFFFAFBFBFAFAFAFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFFAFAFAFBFBFBFEFEFEFEFE
        FEFEFEFEFEFEFEFDFDFDFEFEFEFEFEFEFDFDFDFEFEFEFEFEFEFEFEFEFDFDFDFE
        FEFEFEFEFEFEFEFEFEFEFEFAFAFAFAFAFAFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFF8F8F7FAFCFCA4815EBEA3
        83BEA283BCA080FDFFFFA4815EBCA07FFDFFFFA4815FBEA282BCA080FDFFFFA4
        815FBEA283BEA282BB9F7EF9FBFBF8F8F7FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFF7F7F7FBFBFB7A7A7A9C9C
        9C9B9B9B999999FEFEFE7A7A7A999999FEFEFE7A7A7A9B9B9B999999FEFEFE7A
        7A7A9B9B9B9B9B9B989898FAFAFAF7F7F7FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFFAFAFAFBFBFCFDFFFFFEFF
        FFFDFFFFFCFEFFFCFDFEFDFFFFFCFEFFFCFDFEFDFFFFFDFFFFFCFEFFFCFDFEFD
        FFFFFEFFFFFDFFFFFCFEFFFAFBFBFAFAFAFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFFAFAFAFBFBFBFEFEFEFEFE
        FEFEFEFEFEFEFEFDFDFDFEFEFEFEFEFEFDFDFDFEFEFEFEFEFEFEFEFEFDFDFDFE
        FEFEFEFEFEFEFEFEFEFEFEFAFAFAFAFAFAFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFF8F8F7FAFCFCA4815EBEA3
        83BEA283BCA080FDFFFFA4815EBCA07FFDFFFFA4815FBEA282BCA080FDFFFFA4
        815FBEA283BEA282BB9F7EF9FBFBF8F8F7FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFF7F7F7FBFBFB7A7A7A9C9C
        9C9B9B9B999999FEFEFE7A7A7A999999FEFEFE7A7A7A9B9B9B999999FEFEFE7A
        7A7A9B9B9B9B9B9B989898FAFAFAF7F7F7FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFF6F5F5F8F8FAFCFFFFFEFF
        FFFDFFFFFCFFFFFBFDFFFCFEFFFBFDFFFBFDFFFCFFFFFCFEFFFCFEFFFCFEFFFC
        FEFFFCFEFFFDFFFFFBFDFFF7F7F9F5F4F5FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFF5F5F5F8F8F8FEFEFEFEFE
        FEFEFEFEFEFEFEFDFDFDFEFEFEFDFDFDFDFDFDFEFEFEFEFEFEFEFEFEFEFEFEFE
        FEFEFEFEFEFEFEFEFDFDFDF7F7F7F4F4F4FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFF4F2F3F6F7F8A58360BFA5
        86BFA485BFA485BFA385BEA282F9FBFEA58260BEA282F9FBFFA68361BFA385BE
        A282F9FBFFA68361BDA181F5F6F7F3F2F3FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFF2F2F2F7F7F77C7C7C9E9E
        9E9D9D9D9D9D9D9D9D9D9B9B9BFBFBFB7B7B7B9B9B9BFBFBFB7C7C7C9D9D9D9B
        9B9BFBFBFB7C7C7C9A9A9AF6F6F6F2F2F2FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFF0F0EFF2F4F4F6FAFCF7FB
        FDF6F9FCF7FBFEF7FBFFF6FAFDF5F8FAF6F9FBF6FAFCF6F9FCF7FBFEF6FAFDF5
        F8FAF6F9FBF7FBFDF5F9FBF2F3F3F0F0EFFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFEFEFEFF3F3F3FAFAFAFBFB
        FBF9F9F9FBFBFBFBFBFBFAFAFAF8F8F8F9F9F9FAFAFAF9F9F9FBFBFBFAFAFAF8
        F8F8F9F9F9FBFBFBF9F9F9F2F2F2EFEFEFFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFEEEDEDF1F1F3A78461BFA3
        84F4F6F9A78462C0A586C0A486BEA283F3F5F8A78462C1A586C0A586BFA383F3
        F5F8A78462C0A586BEA282F0F0F2EEEDEDFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFEDEDEDF1F1F17D7D7D9C9C
        9CF6F6F67D7D7D9E9E9E9E9E9E9B9B9BF5F5F57D7D7D9E9E9E9E9E9E9C9C9CF5
        F5F57D7D7D9E9E9E9B9B9BF0F0F0EDEDEDFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFECEBEBEEEEF0F2F4F8F2F5
        F9F1F3F7F2F4F8F2F4F8F2F5F9F1F3F7F1F2F4F2F5F9F3F6FBF2F5FAF1F3F7F1
        F2F4F2F5F9F3F6FAF1F3F6EEEEEFECEBEBFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFEBEBEBEEEEEEF4F4F4F5F5
        F5F3F3F3F4F4F4F4F4F4F5F5F5F3F3F3F2F2F2F5F5F5F7F7F7F6F6F6F3F3F3F2
        F2F2F5F5F5F6F6F6F3F3F3EEEEEEEBEBEBFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFEAE9E8ECECEDA78461C1A5
        86C0A585BFA383EFF0F2A78462BFA383EFF0F1A78562C1A686C0A586BFA383EF
        F0F1A78562C1A586BFA282ECECECE9E9E7FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFE8E8E8ECECEC7D7D7D9E9E
        9E9E9E9E9C9C9CF0F0F07D7D7D9C9C9CF0F0F07E7E7E9F9F9F9E9E9E9C9C9CF0
        F0F07E7E7E9E9E9E9B9B9BECECECE8E8E8FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFE7E6E5E9E9E8EBECECEBEC
        EEEBECEDEBEBECEAEAEBEBECECEBECECEAEAEBEBECEDEBECEEEBECEDEBEBECEA
        EAEBEBECEDEBECEDEAEBEBE9E8E8E7E6E5FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFE5E5E5E8E8E8EBEBEBECEC
        ECECECECEBEBEBEAEAEAEBEBEBEBEBEBEAEAEAECECECECECECECECECEBEBEBEA
        EAEAECECECECECECEAEAEAE8E8E8E5E5E5FFFFFF4C4C4CD0D0D0C8D0D4BEA687
        BFA787C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFE3E2E1E4E3E3E5E4E3E5E4
        E4E5E4E4E5E4E3E5E4E3E5E4E3E5E4E3E5E4E3E5E4E4E5E4E4E5E4E4E5E4E3E5
        E4E3E5E4E4E5E4E4E5E4E3E4E3E2E3E2E1FFFFFF4A4D4EC8D0D4D0D0D09F9F9F
        A0A0A0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFE1E1E1E3E3E3E3E3E3E4E4
        E4E4E4E4E3E3E3E3E3E3E3E3E3E3E3E3E3E3E3E4E4E4E4E4E4E4E4E4E3E3E3E3
        E3E3E4E4E4E4E4E4E3E3E3E2E2E2E1E1E1FFFFFF4C4C4CD0D0D0C8D0D4BD9F79
        F5CA8ABDA585C8D0D4C8D0D4C8D0D44A4D4EFFFFFFE0DFDEE1E0DFE1E0DFE1E0
        DFE1E0DFE1E0DFE1E0DFE1E0DFE1E0DFE1E0DFE1E0DFE1E0DFE1E0DFE1E0DFE1
        E0DFE1E0DFE1E0DFE1E0DFE1E0DFE0DFDEFFFFFF4A4D4EC8D0D4D0D0D0979797
        BBBBBB9E9E9ED0D0D0D0D0D0D0D0D04C4C4CFFFFFFDEDEDEDFDFDFDFDFDFDFDF
        DFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDF
        DFDFDFDFDFDFDFDFDFDFDFDFDFDFDEDEDEFFFFFF4C4C4CD0D0D0C8D0D4BC9F78
        F6C785EDC07DBDA585C8D0D4C8D0D44A4D4EFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4A4D4EC8D0D4D0D0D0969696
        B8B8B8B1B1B19E9E9ED0D0D0D0D0D04C4C4CFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4C4C4CD0D0D0C8D0D4BC9E77
        F3C78AECB468EAB976BDA483C8D0D4585B5D4A4D4E4A4D4E4A4D4E4A4D4E4A4D
        4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A
        4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E5D6162C8D0D4D0D0D0959595
        B9B9B9A3A3A3AAAAAA9C9C9CD0D0D05B5B5B4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C606060D0D0D0C8D0D4BC9E77
        F2C990E6AD5FE8AE60EFCB98BD9C70D6DCDFC8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0959595
        BCBCBC9C9C9C9D9D9DBFBFBF929292DCDCDCD0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4BC9E77
        F2CA98E4A858E5A859EFCC9FBD9C70D6DCDFC8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0959595
        BFBFBF969696979797C2C2C2929292DCDCDCD0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4BC9E77
        F2CE9FE2A14EEED0A8BDA482C8D0D4585B5D4A4D4E4A4D4E4A4D4E4A4D4E4A4D
        4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A
        4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E4A4D4E5D6162C8D0D4D0D0D0959595
        C4C4C48F8F8FC7C7C79C9C9CD0D0D05B5B5B4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C606060D0D0D0C8D0D4BC9E76
        F5D3A9EFD2AFBDA483C8D0D4C8D0D44A4D4EFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4A4D4EC8D0D4D0D0D0959595
        CACACACACACA9C9C9CD0D0D0D0D0D04C4C4CFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4C4C4CD0D0D0C8D0D4BD9F77
        F3DABCBDA483C8D0D4C8D0D4C8D0D44A4D4EFFFFFFFFFFFFFFFFFEFFFFFEFFFF
        FEFFFFFEFFFFFEFFFFFEFFFFFEFFFFFEFFFFFEFFFFFEFFFFFEFFFFFEFFFFFEFF
        FFFEFFFFFEFFFFFEFFFFFEFFFFFEFFFFFFFFFFFF4A4D4EC8D0D4D0D0D0969696
        D3D3D39C9C9CD0D0D0D0D0D0D0D0D04C4C4CFFFFFFFFFFFFFEFEFEFEFEFEFEFE
        FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
        FEFEFEFEFEFEFEFEFEFEFEFEFEFEFFFFFFFFFFFF4C4C4CD0D0D0C8D0D4BEA686
        BFA786C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFFCFCFCFBFBFCFCFCFDFCFC
        FDFCFCFDFBFCFCFBFCFCFCFCFDFBFCFDFBFCFCFCFCFDFCFCFDFBFCFDFBFCFCFC
        FCFDFCFCFDFCFCFDFBFCFCFBFBFBFCFCFCFFFFFF4A4D4EC8D0D4D0D0D09F9F9F
        9F9F9FD0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFFCFCFCFBFBFBFCFCFCFCFC
        FCFCFCFCFBFBFBFBFBFBFCFCFCFCFCFCFBFBFBFCFCFCFCFCFCFCFCFCFBFBFBFC
        FCFCFCFCFCFCFCFCFBFBFBFBFBFBFCFCFCFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFFAF9F9FBFAFBFDFDFFFEFE
        FFFDFEFFFCFDFEFCFCFDFDFDFFFCFDFFFCFCFDFDFEFFFDFEFFFCFDFFFCFCFDFD
        FEFFFEFEFFFDFEFFFCFDFEFAFAFAFAF9F9FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFF9F9F9FAFAFAFDFDFDFEFE
        FEFEFEFEFDFDFDFCFCFCFDFDFDFDFDFDFCFCFCFEFEFEFEFEFEFDFDFDFCFCFCFE
        FEFEFEFEFEFEFEFEFDFDFDFAFAFAF9F9F9FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFF7F7F7F9FBFCA4815EBEA3
        84BEA283BCA080FCFEFFA4815EBCA07FFCFEFFA4815FBEA283BCA080FCFEFFA4
        815FBEA283BEA283BB9F7EF8F9FBF7F7F7FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFF7F7F7FBFBFB7A7A7A9C9C
        9C9B9B9B999999FEFEFE7A7A7A999999FEFEFE7A7A7A9B9B9B999999FEFEFE7A
        7A7A9B9B9B9B9B9B989898F9F9F9F7F7F7FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFF4F5F4F7F8F8FBFFFFFCFF
        FFFCFFFFFBFFFFFAFDFFFBFEFFFAFDFFFAFDFFFBFFFFFBFEFFFBFEFFFAFEFFFB
        FEFFFBFEFFFCFFFFFAFDFFF6F7F7F4F5F4FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFF4F4F4F7F7F7FEFEFEFEFE
        FEFEFEFEFEFEFEFDFDFDFDFDFDFDFDFDFDFDFDFEFEFEFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFEFEFEFDFDFDF6F6F6F4F4F4FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFF2F1F2F5F6F7A68361C0A5
        86BFA486BFA485BFA385BEA282F8F9FDA58360BEA283F8FAFEA68361BFA485BE
        A282F8FAFEA68361BEA181F4F4F6F2F1F2FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFF1F1F1F6F6F67C7C7C9E9E
        9E9E9E9E9D9D9D9D9D9D9B9B9BFAFAFA7C7C7C9B9B9BFAFAFA7C7C7C9D9D9D9B
        9B9BFAFAFA7C7C7C9A9A9AF4F4F4F1F1F1FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFEFEFEEF1F3F3F5F9FBF6FA
        FCF5F8FBF6FAFDF6FAFEF5F9FCF4F7F9F5F8F9F5F9FBF5F8FBF6FAFCF5F9FCF4
        F7F9F5F8FAF6FAFCF4F7F9F1F2F2EFEFEEFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFEEEEEEF2F2F2F9F9F9FAFA
        FAF8F8F8FAFAFAFAFAFAF9F9F9F7F7F7F7F7F7F9F9F9F8F8F8FAFAFAF9F9F9F7
        F7F7F8F8F8FAFAFAF7F7F7F1F1F1EEEEEEFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFEDECEBF0F0F1A78461BFA3
        84F3F5F7A78462C1A586C0A586BFA283F2F4F6A78462C1A586C0A586BFA383F2
        F4F6A78462C1A586BEA282EFEFF0EDECEBFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFEBEBEBF0F0F07D7D7D9C9C
        9CF5F5F57D7D7D9E9E9E9E9E9E9C9C9CF4F4F47D7D7D9E9E9E9E9E9E9C9C9CF4
        F4F47D7D7D9E9E9E9B9B9BEFEFEFEBEBEBFFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFE9EAE9ECEDEEF0F3F6F0F4
        F6EFF2F5F0F3F6F0F3F5F0F4F7EFF2F5EEF1F2F0F4F6F1F5F8F0F4F7EFF2F4EE
        F1F2F0F4F6F0F4F7EFF2F4EBEDEDE9E9E9FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFE9E9E9EDEDEDF3F3F3F4F4
        F4F2F2F2F3F3F3F3F3F3F4F4F4F2F2F2F0F0F0F4F4F4F5F5F5F4F4F4F2F2F2F0
        F0F0F4F4F4F4F4F4F2F2F2ECECECE9E9E9FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFE7E6E5EAEAEBA88462C1A6
        86C1A586BFA383EDEEF0A88562C0A384EDEEEFA88562C1A687C1A586BFA383ED
        EDEFA88562C1A586BFA383EAE9EAE7E6E5FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFE5E5E5EAEAEA7D7D7D9F9F
        9F9E9E9E9C9C9CEEEEEE7E7E7E9D9D9DEEEEEE7E7E7E9F9F9F9E9E9E9C9C9CED
        EDED7E7E7E9E9E9E9C9C9CE9E9E9E5E5E5FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFE3E4E3E6E7E6E8EAEAE8EA
        EBE8EAEBE7E9EAE7E8E9E8EAEAE7E9EAE7E8E9E8EAEAE8EAEBE8EAEBE7E9EAE7
        E8E9E8EAEAE8EAEBE7E9E9E5E6E6E3E4E3FFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFE3E3E3E6E6E6E9E9E9EAEA
        EAEAEAEAE9E9E9E8E8E8E9E9E9E9E9E9E8E8E8E9E9E9EAEAEAEAEAEAE9E9E9E8
        E8E8E9E9E9EAEAEAE8E8E8E5E5E5E3E3E3FFFFFF4C4C4CD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44A4D4EFFFFFFE1E0DFE3E2E1E3E2E2E3E3
        E2E3E2E2E3E2E2E3E2E1E3E2E2E3E2E2E3E2E1E3E2E2E3E3E2E3E2E2E3E2E2E3
        E2E1E3E2E2E3E2E2E3E2E2E3E2E1E1E0DFFFFFFF4A4D4EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D04C4C4CFFFFFFDFDFDFE1E1E1E2E2E2E2E2
        E2E2E2E2E2E2E2E1E1E1E2E2E2E2E2E2E1E1E1E2E2E2E2E2E2E2E2E2E2E2E2E1
        E1E1E2E2E2E2E2E2E2E2E2E1E1E1DFDFDFFFFFFF4C4C4CD0D0D0}
      Margin = 2
      NumGlyphs = 2
    end
    object BtnEsci: TBitBtn
      Left = 7
      Top = 145
      Width = 203
      Height = 38
      Caption = '*Esci'
      ModalResult = 3
      ParentShowHint = False
      ShowHint = True
      TabOrder = 6
      OnMouseMove = AzzeraTimerMouseMove
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
  object GBStatoBaia: TGroupBox
    Left = 10
    Top = 110
    Width = 201
    Height = 111
    Caption = '*Stato baia:'
    Color = clBtnFace
    ParentColor = False
    TabOrder = 2
    OnMouseMove = AzzeraTimerMouseMove
    object CBEsclM: TCheckBox
      Left = 10
      Top = 52
      Width = 186
      Height = 17
      Caption = '*Esclusione'
      TabOrder = 2
      OnClick = AzzeraTimerClick
    end
    object CBEsclCom: TCheckBox
      Left = 10
      Top = 36
      Width = 186
      Height = 17
      Caption = '*Esclusione comunicazioni'
      TabOrder = 1
      OnClick = AzzeraTimerClick
    end
    object CBNoMagaz: TCheckBox
      Left = 10
      Top = 68
      Width = 186
      Height = 18
      Caption = '*Manca deposito/prelievo'
      Enabled = False
      TabOrder = 3
      OnClick = AzzeraTimerClick
    end
    object CBNoTgvDisp: TCheckBox
      Left = 10
      Top = 84
      Width = 186
      Height = 18
      Caption = '*Tgv non disponibile'
      Enabled = False
      TabOrder = 4
      OnClick = AzzeraTimerClick
    end
    object CBBlSovrap: TCheckBox
      Left = 10
      Top = 20
      Width = 186
      Height = 17
      Caption = '*Blocco sovrapp. in rulliera'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnClick = AzzeraTimerClick
    end
  end
  object TimerClose: TTimer
    Enabled = False
    Interval = 90000
    OnTimer = TimerCloseTimer
    Left = 285
    Top = 5
  end
  object PrintDialog: TPrintDialog
    Left = 284
    Top = 50
  end
end
