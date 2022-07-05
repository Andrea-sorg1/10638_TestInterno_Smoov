object FormGruppiBa: TFormGruppiBa
  Left = 1121
  Top = 327
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'GRUPPI BAIE'
  ClientHeight = 239
  ClientWidth = 308
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object CBGruppo: TComboBox
    Left = 6
    Top = 11
    Width = 297
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
    TabOrder = 0
    OnChange = CBGruppoChange
    Items.Strings = (
      '0 - VUOTO'
      '1 - CRUDO'
      '2 - ESSICCATO')
  end
  object GbGroupConfig: TGroupBox
    Left = 6
    Top = 35
    Width = 295
    Height = 172
    TabOrder = 1
    object Label1: TLabel
      Left = 5
      Top = 20
      Width = 21
      Height = 13
      Caption = 'Tipe'
    end
    object Label2: TLabel
      Left = 5
      Top = 50
      Width = 31
      Height = 13
      Caption = 'Priority'
    end
    object Label3: TLabel
      Left = 3
      Top = 96
      Width = 74
      Height = 13
      Caption = 'Max Lgv in wait'
    end
    object Label4: TLabel
      Left = 3
      Top = 123
      Width = 87
      Height = 13
      Caption = 'Max Lgv in upload'
    end
    object Label5: TLabel
      Left = 3
      Top = 148
      Width = 101
      Height = 13
      Caption = 'Max Lgv in download'
    end
    object Bevel1: TBevel
      Left = 2
      Top = 84
      Width = 288
      Height = 14
      Shape = bsTopLine
    end
    object Label6: TLabel
      Left = 9
      Top = 78
      Width = 96
      Height = 13
      Caption = 'Lgv assignament'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object CBTipo: TComboBox
      Left = 48
      Top = 16
      Width = 157
      Height = 21
      Style = csDropDownList
      Color = 14663679
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      TabOrder = 0
      Items.Strings = (
        '00 - NONE'
        '01 - CONVEYOR'
        '02 - GROUND STORAGE'
        '03 - SHELF STORAGE')
    end
    object PanCol: TPanel
      Left = 218
      Top = 15
      Width = 65
      Height = 26
      TabOrder = 1
      OnDblClick = PanColDblClick
    end
    object EdPriorita: TMaskEdit
      Tag = 1
      Left = 48
      Top = 45
      Width = 21
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
      TabOrder = 2
      Text = '00'
    end
    object MaskEdit1: TMaskEdit
      Tag = 1
      Left = 120
      Top = 90
      Width = 21
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
      TabOrder = 3
      Text = '00'
    end
    object MaskEdit2: TMaskEdit
      Tag = 1
      Left = 120
      Top = 117
      Width = 21
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
    end
    object MaskEdit3: TMaskEdit
      Tag = 1
      Left = 120
      Top = 142
      Width = 21
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
      TabOrder = 5
      Text = '00'
    end
  end
  object BtnOk: TBitBtn
    Left = 37
    Top = 212
    Width = 106
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 2
    OnClick = BtnOkClick
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    NumGlyphs = 2
  end
  object BtnClose: TBitBtn
    Left = 165
    Top = 212
    Width = 106
    Height = 25
    TabOrder = 3
    Kind = bkClose
  end
  object ColorDialog1: TColorDialog
    Ctl3D = True
    Left = 218
    Top = 138
  end
end
