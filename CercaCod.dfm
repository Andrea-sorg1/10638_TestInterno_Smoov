object FormCercaCod: TFormCercaCod
  Left = 104
  Top = 190
  BorderStyle = bsDialog
  Caption = 'Seleziona record'
  ClientHeight = 223
  ClientWidth = 382
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnActivate = FormActivate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Edit1: TEdit
    Left = 5
    Top = 5
    Width = 266
    Height = 21
    CharCase = ecUpperCase
    Color = clCream
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    MaxLength = 10
    ParentFont = False
    TabOrder = 0
    Text = 'EDIT1'
    OnChange = Edit1Change
    OnEnter = Edit1Enter
    OnExit = Edit1Exit
  end
  object Edit2: TEdit
    Left = 5
    Top = 30
    Width = 266
    Height = 21
    AutoSelect = False
    CharCase = ecUpperCase
    Color = clCream
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    MaxLength = 30
    ParentFont = False
    TabOrder = 1
    Text = 'EDIT1'
    OnChange = Edit2Change
    OnEnter = Edit2Enter
    OnExit = Edit2Exit
  end
  object BtnOk: TButton
    Left = 280
    Top = 5
    Width = 91
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 2
    OnClick = BtnOkClick
  end
  object BtnCancel: TButton
    Left = 280
    Top = 35
    Width = 91
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
  end
  object CercaGrid: TStringGrid
    Left = 5
    Top = 55
    Width = 266
    Height = 161
    Color = 16744703
    ColCount = 3
    DefaultColWidth = 120
    DefaultRowHeight = 13
    FixedCols = 0
    RowCount = 1000
    FixedRows = 0
    Font.Charset = ANSI_CHARSET
    Font.Color = clBlue
    Font.Height = -9
    Font.Name = 'Small Fonts'
    Font.Style = []
    Options = [goHorzLine, goEditing, goRowSelect]
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 4
    OnClick = CercaGridClick
    OnDblClick = CercaGridDblClick
  end
  object RGOrdine: TRadioGroup
    Left = 280
    Top = 70
    Width = 91
    Height = 81
    Caption = 'Ordinamento:'
    ItemIndex = 0
    Items.Strings = (
      'Id'
      'Codice'
      'Descrizione')
    TabOrder = 5
    OnClick = RGOrdineClick
  end
end
