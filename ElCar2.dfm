object FormElBox: TFormElBox
  Left = 595
  Top = 181
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'List'
  ClientHeight = 392
  ClientWidth = 105
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
  object GridElenco: TStringGrid
    Left = 9
    Top = 5
    Width = 86
    Height = 291
    Color = 16756912
    ColCount = 1
    DefaultRowHeight = 17
    FixedCols = 0
    RowCount = 50
    FixedRows = 0
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clPurple
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goEditing]
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object BtnOk: TBitBtn
    Left = 8
    Top = 335
    Width = 89
    Height = 25
    TabOrder = 1
    OnClick = BtnOkClick
    Kind = bkOK
  end
  object BtnEsci: TBitBtn
    Left = 8
    Top = 363
    Width = 89
    Height = 25
    Caption = 'Esci'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    Kind = bkAbort
  end
  object BtnRESET: TBitBtn
    Left = 8
    Top = 305
    Width = 89
    Height = 25
    Caption = 'RESET'
    TabOrder = 3
    OnClick = BtnRESETClick
  end
end
