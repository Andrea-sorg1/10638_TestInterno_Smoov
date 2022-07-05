object FormMappa: TFormMappa
  Left = 47
  Top = 108
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Gestione mapping LASER'
  ClientHeight = 556
  ClientWidth = 766
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = True
  Position = poScreenCenter
  ShowHint = True
  OnActivate = FormActivate
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 5
    Top = 0
    Width = 759
    Height = 216
    Caption = '*Origine del riferimento Laser:'
    TabOrder = 0
    object Label1: TLabel
      Left = 6
      Top = 22
      Width = 154
      Height = 13
      Caption = '*Angolo Punto Logico su origine '
    end
    object Label2: TLabel
      Left = 6
      Top = 48
      Width = 153
      Height = 13
      Caption = '*X Punto Logico su origine (mm.)'
    end
    object Label3: TLabel
      Left = 6
      Top = 73
      Width = 156
      Height = 13
      Caption = '*Y Punto Logico su origine (mm.) '
    end
    object Label9: TLabel
      Left = 6
      Top = 118
      Width = 161
      Height = 13
      Caption = '*Angolo di sfasamento  con  Laser'
    end
    object Label10: TLabel
      Left = 8
      Top = 96
      Width = 106
      Height = 13
      Caption = '*Lunghezza asse TGV'
    end
    object Label15: TLabel
      Left = 6
      Top = 140
      Width = 118
      Height = 13
      Caption = '*Visualizza Riflettori/TGV'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label16: TLabel
      Left = 3
      Top = 162
      Width = 175
      Height = 13
      Caption = '* Visualizza Riflettori/Zona  (99=tutte)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object GridCoordinate: TStringGrid
      Left = 405
      Top = 10
      Width = 350
      Height = 150
      TabStop = False
      Color = clBtnFace
      DefaultRowHeight = 17
      RowCount = 2000
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ScrollBars = ssVertical
      TabOrder = 3
      OnDrawCell = GridCoordinateDrawCell
      ColWidths = (
        64
        64
        64
        64
        64)
      RowHeights = (
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17
        17)
    end
    object EditRot: TEdit
      Left = 180
      Top = 18
      Width = 56
      Height = 21
      Hint = 
        'Angolo (espresso in gradi ) che assume il TGV nel nodo durante i' +
        'l mapping zona 0'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = 'EditRot'
    end
    object EditX0: TEdit
      Left = 180
      Top = 43
      Width = 56
      Height = 21
      Hint = 
        'Coordinata X (espresso in mm. ) che assume il TGV nel nodo duran' +
        'te il mapping zona 0'
      TabOrder = 1
      Text = 'Edit1'
    end
    object EditY0: TEdit
      Left = 180
      Top = 68
      Width = 56
      Height = 21
      Hint = 
        'Coordinata Y (espresso in mm. ) che assume il TGV nel nodo duran' +
        'te il mapping zona 0'
      TabOrder = 2
      Text = 'Edit1'
    end
    object BtnTest: TButton
      Left = 249
      Top = 164
      Width = 70
      Height = 25
      Caption = '*&Test Plc'
      TabOrder = 4
      Visible = False
      OnClick = BtnTestClick
    end
    object TextGRDOffset: TStaticText
      Left = 180
      Top = 115
      Width = 56
      Height = 20
      Hint = 
        'Angolo (espresso in gradi ) di sfasamento tra il riferimento LAS' +
        'ER e un riferimento LAYOUT TRADIZIONALE'
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'TextGRDOffset'
      TabOrder = 5
    end
    object EditScostPLogico: TEdit
      Left = 180
      Top = 92
      Width = 56
      Height = 21
      Hint = 
        'Scostamento tra Punto logico e Testa laser (recuperato da DM210 ' +
        'PLC)'
      Color = clBtnFace
      ReadOnly = True
      TabOrder = 6
      Text = 'EditPLogico'
    end
    object Panel1: TPanel
      Left = 370
      Top = 10
      Width = 29
      Height = 29
      BorderStyle = bsSingle
      Color = 11777709
      TabOrder = 7
      object BtnLed: TSpeedButton
        Left = 0
        Top = 0
        Width = 25
        Height = 25
        Flat = True
        Glyph.Data = {
          36060000424D3606000000000000360000002800000020000000100000000100
          18000000000000060000C40E0000C40E00000000000000000000C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
          D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
          D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
          D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4A2A2C53232820A0A740F0F7A3A3A89A6A6CAC8D0D4C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4A4C1AB387A4812
          6829146C2B3F8350A7C6AFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D46666A205057E0B0BA70C0CA80808A50B0BA612128B6D6DA8C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4699C760E712814933414
          9536139234159536167E316EA17BC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4A4A4CA0F0F892A2AC73434CE3434CD2929C21717B00707A40F0F8AA5A5
          C9C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4A6C5AD147A2E23AE4929B54D29
          B44D23AA471A9B3B139334157D30A6C6AFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D43D3D8D3333CF4F4FEA5B5BF65B5BF54D4DE83737D01A1AB30909A43838
          88C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D43D834F29B54E39CD5F3ED7663E
          D76637CA5D2CB7501B9D3D1392343C7F4DC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D41D1D885353F07778FF9295FF9193FF7474FF5151EC3131CA0C0CA70808
          73C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D41977333BD3624CE87564F18963
          F1884BE77439CE6027B04B159436106728C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D423238F6869FFA2A5FFCED3FFCDD1FF9DA1FF6767FC3F3FD91717B30303
          6DC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D41D7D3743E46E78F498B4F8C5B2
          F7C371F39444E06C2FBE541A9D3D0C6122C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D44A499B6768F8B3B7FFEAECFFE5E9FFADB1FF6E70FF4343DD1919B43131
          7EC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4458D5843E16C8EF5A8DDFBE5D9
          FBE286F5A348E47032C1581A9F3E367547C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4AEAED43C3CB39B9DFFC4C9FFC2C7FF979AFF6463FB3D3DDB0F0F8AA3A3
          C8C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4ABCDB42AA0486EF691A5F7BBA2
          F6B869F28D42DD6B2FBF5413792EA6C3ADC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D47D7DB94041B4797BF98485FF6B6AFF4747E41B1B956C6CA8C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D476B0852BA34B51E87855
          EF7E44E56F34C75A1A83366C9F79C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4B1B1D54E4EA12C2C99272792454594A8A8CDC8D0D4C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4ADCFB548925B22
          863C1F813A418853A8C8B0C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
          D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
          D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
          D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
          D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4}
        Margin = 3
        NumGlyphs = 2
      end
    end
    object EdLaserMappaRiflettoriTGV: TEdit
      Left = 220
      Top = 136
      Width = 16
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 1
      ParentFont = False
      TabOrder = 8
      Text = '0'
      OnChange = EdLaserMappaRiflettoriTGVChange
    end
    object EdLaserMappaRiflettoriZona: TEdit
      Left = 217
      Top = 159
      Width = 19
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      MaxLength = 2
      ParentFont = False
      TabOrder = 9
      Text = '00'
      OnChange = EdLaserMappaRiflettoriZonaChange
    end
    object CBVisualizzaTrasparente: TCheckBox
      Left = 245
      Top = 70
      Width = 151
      Height = 17
      Alignment = taLeftJustify
      Caption = '*Visualizza solo punto logico'
      TabOrder = 10
      OnClick = CBVisualizzaTrasparenteClick
    end
    object BtnLoad: TBitBtn
      Left = 405
      Top = 165
      Width = 170
      Height = 40
      Hint = 'Rileggi la mappa dei nodi da PLC  (DM1000..1999)'
      Caption = '*&Carica Mappa da PLC'
      TabOrder = 11
      OnClick = BtnLoadClick
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D47F7F7F
        7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
        7F7F7F7FC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D07F7F7F
        7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
        7F7F7F7FD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C06060606F6F6FC8D0D4C8D0D4C8D0D40000FF0000FF0000FF0000FF0000FF00
        00FF0808FF4040FF9F9FFFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C06060606F6F6FD0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C515151797979BBBBBBD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C06F6F6FC8D0D4C8D0D40000FF0000FF0000FF0000FF0000FF00
        00FF0000FF0000FF0000FF4848FFC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C06F6F6FD0D0D0D0D0D04C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C7E7E7ED0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C06F6F6FC8D0D40000FF0000FF0000FF0000FF0000FF00
        00FF0000FF0000FF0000FF0000FF4040FFC8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C06F6F6FD0D0D04C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4C4C797979D0D0D0D0D0D0D0D0D0C8D0D4404040
        8080804040408080806060A0606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060C8D0D44040FF4040FF4040FF4040FF4040FF40
        40FF3030FF0000FF0000FF0000FF0000FF9797FFC8D0D4C8D0D4D0D0D0404040
        808080404040808080737373606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060D0D0D079797979797979797979797979797979
        79796D6D6D4C4C4C4C4C4C4C4C4C4C4C4CB6B6B6D0D0D0D0D0D0C8D0D4404040
        80808040404080808060A060606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D48F8FFF0000FF0000FF0000FF3030FFC8D0D4C8D0D4D0D0D0404040
        808080404040808080858585606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0B0B0B04C4C4C4C4C4C4C4C4C6D6D6DD0D0D0D0D0D0C8D0D4404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D43838FF0000FF0000FF0808FFC8D0D4C8D0D4D0D0D0404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D07373734C4C4C4C4C4C515151D0D0D0D0D0D0C8D0D4404040
        8080804040408080808080804040404040404040404040404040404040404040
        40404040C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46060FF0000FF0000FF0000FFC8D0D4C8D0D4D0D0D0404040
        8080804040408080808080804040404040404040404040404040404040404040
        40404040C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D08F8F8F4C4C4C4C4C4C4C4C4CD0D0D0D0D0D0C8D0D4404040
        60A06040404040404080808040404060A06040404060A06080808040404060A0
        60404040C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46060FF0000FF0000FF0000FFC8D0D4C8D0D4D0D0D0404040
        8585854040404040408080804040408585854040408585858080804040408585
        85404040C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D08F8F8F4C4C4C4C4C4C4C4C4CD0D0D0D0D0D0C8D0D4404040
        80808040404080808060606040404060A0604040408080808080804040408080
        80404040C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46060FF0000FF0000FF0000FFC8D0D4C8D0D4D0D0D0404040
        8080804040408080806060604040408585854040408080808080804040408080
        80404040C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D08F8F8F4C4C4C4C4C4C4C4C4CD0D0D0D0D0D0C8D0D4404040
        6060605050506060606060605050506060608080803030306060605050506060
        60808080909090C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46060FF0000FF0000FF0000FFC8D0D4C8D0D4D0D0D0404040
        6060605050506060606060605050506060608080803030306060605050506060
        60808080909090C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D08F8F8F4C4C4C4C4C4C4C4C4CD0D0D0D0D0D0C8D0D4C8D0D4
        5F5F5FA0A0A0707070A0A0A0A0A0A0505050A0A0A0808080707070A0A0A05050
        50A0A0A0808080909090606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46060FF0000FF0000FF0000FFC8D0D4C8D0D4D0D0D0D0D0D0
        5F5F5FA0A0A0707070A0A0A0A0A0A0505050A0A0A0808080707070A0A0A05050
        50A0A0A0808080909090606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D08F8F8F4C4C4C4C4C4C4C4C4CD0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D45F5F5FA0A0A0505050A0A0A0A0A0A0505050A0A0A0808080707070A0A0
        A0505050A0A0A0808080303030C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D45050FF1818FF0000FF0000FF0000FF3838FF9F9FFFD0D0D0D0D0D0
        D0D0D05F5F5FA0A0A0505050A0A0A0A0A0A0505050A0A0A0808080707070A0A0
        A0505050A0A0A0808080303030D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D08484845D5D5D4C4C4C4C4C4C4C4C4C737373BBBBBBC8D0D4C8D0D4
        C8D0D4C8D0D47F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
        7F7F7F7F7F7F7F7F7F7FBFBFBFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D41010FF0000FF0000FF0000FF6060FFC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D07F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
        7F7F7F7F7F7F7F7F7F7FBFBFBFD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D05757574C4C4C4C4C4C4C4C4C8F8F8FD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D41010FF0000FF6060FFC8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D05757574C4C4C8F8F8FD0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D47070FFC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D09A9A9AD0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BAC7CD7C9FB4497798237D9C0D
        799B036F9703638E1C638786A0B2C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C7C7C7A1A1A17B7B7B7C7C7C76
        76766E6E6E646464656565A2A2A2D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4B4C6CE7BA6BC3B7EA0055F931E87B837A6D22B9BCA24
        96C71389BE007BBA35A8DD07477CA6B6C3C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C6C6C6A7A7A7808080646464898989A6A6A69C9C9C97
        97978B8B8B7F7F7FAAAAAA4F4F4FB8B8B8D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4388EB0087DAB309FC659CCEB80F2FF7EF0FF77ECFF71EAFF63
        DBF44EC2E157C0DB67E7FF349FCB597E9AC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D08E8E8E7D7D7D9E9E9EC8C8C8E8E8E8E7E7E7E4E4E4E2E2E2D4
        D4D4BEBEBEBCBCBCDFDFDF9F9F9F828282D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D45898A39FFFFF9AF8FF90F5FF8CF4FF89F4FF7BDAE76DB8C265
        A3AA5C8C9169C9DA6DE6FF5BD5F6245982C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0939393F4F4F4EFEFEFECECECEBEBEBEBEBEBD3D3D3B2B2B29E
        9E9E888888C3C3C3DFDFDFD0D0D05F5F5FD0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D485A9B280D2DEA9F9FF89CFD770A3A874B1B87AC1C884DFEB89
        F6FF85F7FF80F3FF7AECFF7CF3FF065688B7C4CBC8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0A7A7A7CCCCCCF1F1F1C9C9C99E9E9EACACACBABABAD8D8D8EC
        ECECECECECE9E9E9E4E4E4E9E9E95B5B5BC4C4C4D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4B9C8CC5DA4B0B3FCFFA9F7FFADFBFFA1FBFF94F8FF91F7FF85
        E0EC74B7BE68999D7EDBE889F6FF3190B5819FB2C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0C7C7C79F9F9FF4F4F4F0F0F0F3F3F3F1F1F1EEEEEEEDEDEDD9
        D9D9B1B1B1949494D4D4D4ECECEC909090A1A1A1D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4709FAAA8EFF8C1FBFF96D2D88DAFB28FB3B584B1B387
        C9D091EBF693F7FF8DF4FF8CF3FF5FCAE4497B96C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D09C9C9CE9E9E9F5F5F5CCCCCCACACACAFAFAFACACACC3
        C3C3E4E4E4EDEDEDEBEBEBEAEAEAC5C5C57D7D7DD0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4A3BCC37EC0C9D0FFFFB7EEF3AEF7FFB4FEFFC0FFFFBC
        FDFFB6FDFFA7E8F094C9CFA8F5FF93F5FF135F88C7CFD3C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0BBBBBBBBBBBBF9F9F9E9E9E9F1F1F1F5F5F5F7F7F7F6
        F6F6F5F5F5E2E2E2C4C4C4EFEFEFECECEC626262CFCFCFD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C5CDD1659AA5D3FFFFD7FEFFC4E6E9A0CBCE83ACB092
        BBBD97BDC0A0D3D8AFE3E9B7F8FFACFBFF2081A2A3B8C2C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0CDCDCD979797F9F9F9F9F9F9E3E3E3C6C6C6A8A8A8B6
        B6B6B9B9B9CECECEDEDEDEF2F2F2F3F3F37F7F7FB8B8B8D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D45B948AB2E2E7E4FFFFBFE7E9D7F8FBDBFFFFCA
        FFFFB9FEFFB0FDFFADF8FFAAF6FFAAF9FF53B5CD6B95A6C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D08A8A8ADEDEDEFBFBFBE3E3E3F5F5F5FAFAFAF8
        F8F8F6F6F6F4F4F4F1F1F1F0F0F0F1F1F1B1B1B1959595D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D484B7BDEEFFFFE8FFFFDBF1F3C8DDDEBF
        D3D4B5C6C5B5C9CAA9B8B9BAD8DAD1FDFF93EAF934728BC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0B2B2B2FDFDFDFCFCFCEFEFEFDADADAD1
        D1D1C3C3C3C7C7C7B6B6B6D5D5D5F8F8F8E4E4E4727272D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D477A1A7E9FFFFEEFAFAD8E4E2E5F6F5ED
        FFFFECFFFFE9FFFFE3FFFFD9FFFFCFFFFFC0FFFF10708EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D09E9E9EFCFCFCF8F8F8E2E2E2F3F3F3FC
        FCFCFCFCFCFCFCFCFBFBFBFAFAFAF9F9F9F7F7F76E6E6ED0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D495B0B6C2E6E7F9FFFFE5FDFECEEEEFB7
        DDE19FCDD289BFC678AFB768A2AB639CA66598A487ADB6C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0AEAEAEE2E2E2FEFEFEFAFAFAEAEAEAD9
        D9D9C9C9C9BABABAABABAB9E9E9E989898959595ABABABD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4B5C4C96C959F6C949D7497A082A2A98D
        AAB09AB1B8A6BAC0B2C2C7C0CBCFC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C3C3C3939393929292959595A0A0A0A8
        A8A8B0B0B0B9B9B9C1C1C1CACACAD0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      NumGlyphs = 2
    end
    object BtnWrite: TBitBtn
      Left = 585
      Top = 165
      Width = 170
      Height = 40
      Hint = 'Scrivi la mappa dei nodi su PLC  (DM1000..1999)'
      Caption = '*&Scarica Mappa su PLC'
      TabOrder = 12
      OnClick = BtnWriteClick
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D47F7F7F
        7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
        7F7F7F7FC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D07F7F7F
        7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
        7F7F7F7FD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C06060606F6F6FC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C06060606F6F6FD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C06F6F6FC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C06F6F6FD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C06F6F6FC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C06F6F6FD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        8080804040408080806060A0606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080737373606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        80808040404080808060A060606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080858585606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        808080404040808080808080606060C0C0C0606060C0C0C0C0C0C0606060C0C0
        C0606060C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        8080804040408080808080804040404040404040404040404040404040404040
        40404040C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        8080804040408080808080804040404040404040404040404040404040404040
        40404040C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        60A06040404040404080808040404060A06040404060A06080808040404060A0
        60404040C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        8585854040404040408080804040408585854040408585858080804040408585
        85404040C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        80808040404080808060606040404060A0604040408080808080804040408080
        80404040C0C0C0C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        8080804040408080806060604040408585854040408080808080804040408080
        80404040C0C0C0C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4404040
        6060605050506060606060605050506060608080803030306060605050506060
        60808080909090C0C0C0606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0404040
        6060605050506060606060605050506060608080803030306060605050506060
        60808080909090C0C0C0606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        5F5F5FA0A0A0707070A0A0A0A0A0A0505050A0A0A0808080707070A0A0A05050
        50A0A0A0808080909090606060C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        5F5F5FA0A0A0707070A0A0A0A0A0A0505050A0A0A0808080707070A0A0A05050
        50A0A0A0808080909090606060D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D45F5F5FA0A0A0505050A0A0A0A0A0A0505050A0A0A0808080707070A0A0
        A0505050A0A0A0808080303030C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D05F5F5FA0A0A0505050A0A0A0A0A0A0505050A0A0A0808080707070A0A0
        A0505050A0A0A0808080303030D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D47F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
        7F7F7F7F7F7F7F7F7F7FBFBFBFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D07F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F7F
        7F7F7F7F7F7F7F7F7F7FBFBFBFD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D47070FFC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D09A9A9AD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D46060FF0000FF1010FFC8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D08F8F8F4C4C4C575757D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D46060FF0000FF0000FF0000FF1010FFC8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BAC7CD7C9FB4497798237D9C0D
        799B036F9703638E1C638786A0B2C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D08F8F8F4C4C4C4C4C4C4C4C4C575757D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C7C7C7A1A1A17B7B7B7C7C7C76
        76766E6E6E646464656565A2A2A2D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D49F9FFF3838FF0000FF0000FF0000FF1818FF5050FFC8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4B4C6CE7BA6BC3B7EA0055F931E87B837A6D22B9BCA24
        96C71389BE007BBA35A8DD07477CA6B6C3C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0BBBBBB7373734C4C4C4C4C4C4C4C4C5D5D5D848484D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0C6C6C6A7A7A7808080646464898989A6A6A69C9C9C97
        97978B8B8B7F7F7FAAAAAA4F4F4FB8B8B8D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40000FF0000FF0000FF6060FFC8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4388EB0087DAB309FC659CCEB80F2FF7EF0FF77ECFF71EAFF63
        DBF44EC2E157C0DB67E7FF349FCB597E9AC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C8F8F8FD0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D08E8E8E7D7D7D9E9E9EC8C8C8E8E8E8E7E7E7E4E4E4E2E2E2D4
        D4D4BEBEBEBCBCBCDFDFDF9F9F9F828282D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40000FF0000FF0000FF6060FFC8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D45898A39FFFFF9AF8FF90F5FF8CF4FF89F4FF7BDAE76DB8C265
        A3AA5C8C9169C9DA6DE6FF5BD5F6245982C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C8F8F8FD0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0939393F4F4F4EFEFEFECECECEBEBEBEBEBEBD3D3D3B2B2B29E
        9E9E888888C3C3C3DFDFDFD0D0D05F5F5FD0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40000FF0000FF0000FF6060FFC8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D485A9B280D2DEA9F9FF89CFD770A3A874B1B87AC1C884DFEB89
        F6FF85F7FF80F2FF7AECFF7CF3FF065688B7C4CBC8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C8F8F8FD0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0A7A7A7CCCCCCF1F1F1C9C9C99E9E9EACACACBABABAD8D8D8EC
        ECECECECECE8E8E8E4E4E4E9E9E95B5B5BC4C4C4D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40000FF0000FF0000FF6060FFC8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4B9C8CC5DA4B0B3FCFFA9F7FFADFBFFA1FBFF94F8FF91F7FF85
        E0EC74B7BE68999D7EDBE889F6FF3190B5819FB2C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C8F8F8FD0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0C7C7C79F9F9FF4F4F4F0F0F0F3F3F3F1F1F1EEEEEEEDEDEDD9
        D9D9B1B1B1949494D4D4D4ECECEC909090A1A1A1D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40000FF0000FF0000FF6060FFC8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4709FAAA8EFF8C1FBFF96D2D88DAFB28FB3B584B1B387
        C9D091EBF693F7FF8DF4FF8CF3FF5FCAE4497B96C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C8F8F8FD0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D09C9C9CE9E9E9F5F5F5CCCCCCACACACAFAFAFACACACC3
        C3C3E4E4E4EDEDEDEBEBEBEAEAEAC5C5C57D7D7DD0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40808FF0000FF0000FF3838FFC8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4A3BCC37EC0C9D0FFFFB7EEF3AEF7FFB4FEFFC0FFFFBC
        FDFFB6FDFFA7E8F094C9CFA8F5FF93F5FF135F88C7CFD3C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D05151514C4C4C4C4C4C737373D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0BBBBBBBBBBBBF9F9F9E9E9E9F1F1F1F5F5F5F7F7F7F6
        F6F6F5F5F5E2E2E2C4C4C4EFEFEFECECEC626262CFCFCFD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D43030FF0000FF0000FF0000FF8F8FFFC8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C5CDD1659AA5D3FFFFD7FEFFC4E6E9A0CBCE83ACB092
        BBBD97BDC0A0D3D8AFE3E9B7F8FFACFBFF2081A2A3B8C2C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D06D6D6D4C4C4C4C4C4C4C4C4CB0B0B0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0CDCDCD979797F9F9F9F9F9F9E3E3E3C6C6C6A8A8A8B6
        B6B6B9B9B9CECECEDEDEDEF2F2F2F3F3F37F7F7FB8B8B8D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D49797FF0000FF0000FF0000FF0000FF3030FF4040FF4040
        FF4040FF4040FF4040FF4040FF5B948AB2E2E7E4FFFFBFE7E9D7F8FBDBFFFFCA
        FFFFB9FEFFB0FDFFADF8FFAAF6FFAAF9FF53B5CD6B95A6C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0B6B6B64C4C4C4C4C4C4C4C4C4C4C4C6D6D6D7979797979
        797979797979797979797979798A8A8ADEDEDEFBFBFBE3E3E3F5F5F5FAFAFAF8
        F8F8F6F6F6F4F4F4F1F1F1F0F0F0F1F1F1B1B1B1959595D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D44040FF0000FF0000FF0000FF0000FF0000FF0000
        FF0000FF0000FF0000FF0000FFC8D0D484B7BDEEFFFFE8FFFFDBF1F3C8DDDEBF
        D3D4B5C6C5B5C9CAA9B8B9BAD8DAD1FDFF93EAF934728BC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D07979794C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4CD0D0D0B2B2B2FDFDFDFCFCFCEFEFEFDADADAD1
        D1D1C3C3C3C7C7C7B6B6B6D5D5D5F8F8F8E4E4E4727272D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44848FF0000FF0000FF0000FF0000FF0000
        FF0000FF0000FF0000FF0000FFC8D0D477A1A7E9FFFFEEFAFAD8E4E2E5F6F5ED
        FFFFECFFFFE9FFFFE3FFFFD9FFFFCFFFFFC0FFFF10708EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D07E7E7E4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4CD0D0D09E9E9EFCFCFCF8F8F8E2E2E2F3F3F3FC
        FCFCFCFCFCFCFCFCFBFBFBFAFAFAF9F9F9F7F7F76E6E6ED0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D49F9FFF4040FF0808FF0000FF0000
        FF0000FF0000FF0000FF0000FFC8D0D495B0B6C2E6E7F9FFFFE5FDFECEEEEFB7
        DDE19FCDD289BFC678AFB768A2AB639CA66598A487ADB6C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0BBBBBB7979795151514C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4CD0D0D0AEAEAEE2E2E2FEFEFEFAFAFAEAEAEAD9
        D9D9C9C9C9BABABAABABAB9E9E9E989898959595ABABABD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4B5C4C96C949E6C949E7497A082A2A98D
        AAB09AB1B8A6BAC0B2C2C7C0CBCFC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C3C3C3929292929292959595A0A0A0A8
        A8A8B0B0B0B9B9B9C1C1C1CACACAD0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      NumGlyphs = 2
    end
  end
  object GroupBox2: TGroupBox
    Left = 5
    Top = 380
    Width = 226
    Height = 154
    Caption = '*Info da LASER:'
    TabOrder = 1
    object Label4: TLabel
      Left = 10
      Top = 21
      Width = 123
      Height = 13
      Caption = '*Nodo del'#39'impianto e  Rot.'
    end
    object Label7: TLabel
      Left = 12
      Top = 47
      Width = 96
      Height = 13
      Caption = '*Laser Punto Logico'
    end
    object Label8: TLabel
      Left = 118
      Top = 47
      Width = 102
      Height = 13
      Caption = '*Layout Punto Logico'
      Visible = False
    end
    object Label12: TLabel
      Left = 73
      Top = 131
      Width = 74
      Height = 13
      Caption = '*Specchi in uso'
    end
    object Label13: TLabel
      Left = 175
      Top = 131
      Width = 17
      Height = 13
      Caption = 'G   '
    end
    object Label14: TLabel
      Left = 13
      Top = 131
      Width = 29
      Height = 13
      Caption = '*Zona'
    end
    object TextXls: TStaticText
      Left = 12
      Top = 64
      Width = 94
      Height = 18
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'X: 00000000'
      TabOrder = 0
    end
    object TextYls: TStaticText
      Left = 12
      Top = 84
      Width = 94
      Height = 18
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'X: 00000000'
      TabOrder = 1
    end
    object TextRotls: TStaticText
      Left = 12
      Top = 104
      Width = 94
      Height = 18
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'X: 00000000'
      TabOrder = 2
    end
    object TextXly: TStaticText
      Left = 119
      Top = 64
      Width = 97
      Height = 18
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'X: 00000000'
      TabOrder = 3
      Visible = False
    end
    object TextYly: TStaticText
      Left = 119
      Top = 84
      Width = 97
      Height = 18
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'X: 00000000'
      TabOrder = 4
      Visible = False
    end
    object TextRotly: TStaticText
      Left = 119
      Top = 104
      Width = 97
      Height = 18
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'X: 00000000'
      TabOrder = 5
      Visible = False
    end
    object TextRotN: TStaticText
      Left = 184
      Top = 19
      Width = 31
      Height = 18
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'TextRotN'
      TabOrder = 6
    end
    object TextNodo: TStaticText
      Left = 141
      Top = 19
      Width = 35
      Height = 18
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'TextNodo'
      TabOrder = 7
    end
    object EditRiflInUso: TEdit
      Left = 146
      Top = 128
      Width = 26
      Height = 21
      Hint = 'Riflettori in uso in questo momento'
      Color = clBtnFace
      ReadOnly = True
      TabOrder = 8
      Text = 'EditPLogico'
    end
    object EditG: TEdit
      Left = 190
      Top = 128
      Width = 26
      Height = 21
      Hint = 'Valore di G attuale'
      Color = clBtnFace
      ReadOnly = True
      TabOrder = 9
      Text = 'EditPLogico'
    end
    object EditZona: TEdit
      Left = 45
      Top = 128
      Width = 26
      Height = 21
      Hint = 'Zona attivata in questo momento'
      Color = clBtnFace
      ReadOnly = True
      TabOrder = 10
      Text = 'EditPLogico'
    end
  end
  object GroupBox3: TGroupBox
    Left = 244
    Top = 220
    Width = 520
    Height = 261
    Caption = '*Gestione mapping zone:'
    TabOrder = 2
    object GridZone: TStringGrid
      Left = 9
      Top = 20
      Width = 507
      Height = 96
      ColCount = 9
      DefaultColWidth = 53
      DefaultRowHeight = 17
      RowCount = 41
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goDrawFocusSelected, goEditing, goTabs]
      ParentFont = False
      ScrollBars = ssVertical
      TabOrder = 0
    end
    object BtnMapping: TButton
      Left = 10
      Top = 120
      Width = 173
      Height = 25
      Hint = 'Attiva la procedura di Mapping di una zona specifica'
      Caption = '*Attivazione mapping'
      TabOrder = 1
      OnClick = BtnMappingClick
    end
    object BtnCancella: TButton
      Left = 188
      Top = 121
      Width = 173
      Height = 24
      Hint = 'Cancellazione di una zona specifica'
      Caption = '*Cancella Zona Mapping'
      TabOrder = 2
      OnClick = BtnCancellaClick
    end
    object BtnVisualizza: TButton
      Left = 365
      Top = 121
      Width = 146
      Height = 24
      Caption = '*Visualizza Dati'
      TabOrder = 3
      OnClick = BtnVisualizzaClick
    end
    object BtnUpload: TBitBtn
      Left = 10
      Top = 160
      Width = 220
      Height = 40
      Hint = 'Carica dal LASER una o tutte le zone di mapping'
      Caption = '*CARICO RIFLETTORI DA TGV'
      TabOrder = 4
      OnClick = BtnUploadClick
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        BACEE07A9CB6608CAA4966881B6CA71166A31B78B71D76B9127ED71680DAB9BC
        C2C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D1D1D19F9F9F8F8F8F6C6C6C7474746E6E6E8080807F7F7F8C8C8C8E8E8EBDBD
        BDD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4CDE7F8
        2895E21191E81392E81C8FE20D91EA0A8FE5098FE60A8FE5088BE40B84DA6078
        95BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE
        BEBEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0E9E9E9
        9F9F9F9C9C9C9D9D9D9A9A9A9C9C9C9999999999999999999696968F8F8F7D7D
        7DBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE
        BEBED0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4CDEBFE
        24A5F70D9AF50C94EF0C95F00D8BDD0B88DA1683CD1180C70A83D20C7CCC0826
        42272623272623272623272623272623272623272623272623272623272623C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0EDEDED
        AEAEAEA5A5A59F9F9FA0A0A09595959292928C8C8C8888888C8C8C8787872A2A
        2A252525252525252525252525252525252525252525252525252525252525D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4CCEAFB
        2482BC0E65980D60950F83D10C38540B2B3C0B1B240B13190B2F570A345E0B1E
        394F504FBEBEBEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0EBEBEB
        8888886A6A6A6666668D8D8D3B3B3B2C2C2C1B1B1B1313133636363B3B3B2323
        234F4F4FBEBEBED0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4CDE7F7
        1F38440A0C060B20291278BF060B0B070B07070807090A0B0915290408090811
        16262B2B272623272623272623272623272623272623272623C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0E8E8E8
        3838380909092020208181810A0A0A0909090707070A0A0A1919190707071111
        112A2A2A252525252525252525252525252525252525252525D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4CAE4F4
        1E323D060B060A1F281276BA03070A010402010402040508071023040708070A
        0B404547E5E5E6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0E5E5E5
        3333330808081F1F1F7E7E7E0707070303030303030505051414140606060909
        09454545E5E5E5D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4BDDDF0
        132C3A07070709212E1276BA0107090003020104020406060612250306060B0E
        0E6F7173C8D0D4C8D0D4C8D0D4C8D0D40000FF0000FF1819F95053ED5053ED50
        53ED5458ED7679EDAFB2EDC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0DFDFDF
        2D2D2D0707072222227E7E7E0606060202020303030505051616160505050D0D
        0D717171D0D0D0D0D0D0D0D0D0D0D0D04C4C4C4C4C4C5B5B5B80808080808080
        80808484849B9B9BC3C3C3D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4BCDBED
        0F2A370606060A21311274B8000606010402010402030804101E30352F2B0B10
        0C717375C8D0D4C8D0D4C8D0D4C8D0D40000FF0000FF0000FF0000FF0000FF00
        00FF0000FF0000FF0000FF4848FFC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0DCDCDC
        2A2A2A0606062323237D7D7D0505050303030303030606062121212E2E2E0E0E
        0E737373D0D0D0D0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C7E7E7ED0D0D0D0D0D0D0D0D0D0D0D0C8D0D4B6D7EA
        0D27350406060A22321070B40006060406060308030209062834419381730C0E
        08757775C8D0D4C8D0D4C8D0D4C8D0D40000FF0000FF0000FF0000FF0000FF00
        00FF0000FF0000FF0000FF0000FF4040FFC8D0D4C8D0D4C8D0D4D0D0D0D8D8D8
        2828280505052424247979790505050505050505050707073636367E7E7E0B0B
        0B767676D0D0D0D0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4C4C797979D0D0D0D0D0D0D0D0D0C8D0D4B7D4E7
        0F24340306060625351172B6000606030804020803050A061A2A395C534C1214
        0F777977C8D0D4C8D0D4C8D0D4C8D0D44040FF4040FF4040FF4040FF4040FF40
        40FF3030FF0000FF0000FF0000FF0000FF9797FFC8D0D4C8D0D4D0D0D0D6D6D6
        2626260505052626267B7B7B0505050606060505050808082C2C2C5151511212
        12787878D0D0D0D0D0D0D0D0D0D0D0D079797979797979797979797979797979
        79796D6D6D4C4C4C4C4C4C4C4C4C4C4C4CB6B6B6D0D0D0D0D0D0C8D0D4B6D2E4
        0E223304060607263C1373BA000708040606040606050A060518290606060E0E
        0D777777C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D48F8FFF0000FF0000FF0000FF3030FFC8D0D4C8D0D4D0D0D0D4D4D4
        2424240505052929297D7D7D0606060505050505050808081A1A1A0606060D0D
        0D777777D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0B0B0B04C4C4C4C4C4C4C4C4C6D6D6DD0D0D0D0D0D0C8D0D4B6D2E4
        10263A0408090828401374BD040E14030C0D040A0B040C0D0A1B2C040A055456
        52ADADADC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D46060FF0000FF0000FF0000FFC8D0D4C8D0D4D0D0D0D4D4D4
        2929290707072B2B2B7E7E7E0E0E0E0B0B0B0909090B0B0B1E1E1E0707075454
        54ADADADD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D08F8F8F4C4C4C4C4C4C4C4C4CD0D0D0D0D0D0C8D0D4B5CCDC
        1F4C72122F4B0B52871283D5176A9A0F5A870E567B0E547C0C3255080D09B8BA
        B9C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D45050FF1818FF0000FF0000FF0000FF3838FF9F9FFFD0D0D0CECECE
        5252523434345959598E8E8E6E6E6E5E5E5E5858585757573838380B0B0BB9B9
        B9D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D08484845D5D5D4C4C4C4C4C4C4C4C4C737373BBBBBBC8D0D4BCBEBE
        202928234058125E921987DB1DC5F016B8EE1CBAEA1DB0E71D517A0C100FB6B8
        B7C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D41010FF0000FF0000FF0000FF6060FFC8D0D4D0D0D0BDBDBD
        272727434343646464939393BEBEBEB5B5B5B6B6B6AFAFAF5757570F0F0FB7B7
        B7D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D05757574C4C4C4C4C4C4C4C4C8F8F8FD0D0D0C8D0D4C8D0D4
        74746D2525181C333F1B96D621CAEE1BBCF01EC5F425C0EF406E8D0E11118788
        89C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D41010FF0000FF6060FFC8D0D4C8D0D4D0D0D0D0D0D0
        7171712121213333339B9B9BC1C1C1B9B9B9C0C0C0BCBCBC7272721010108888
        88D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D05757574C4C4C8F8F8FD0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D48D8D8763645D408DAF3FC7F13FC3F43EC6F753BBE62A65880C0E0F898A
        89C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D47070FFC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D08B8B8B6161618E8E8EC4C4C4C2C2C2C5C5C5BCBCBC6868680E0E0E8989
        89D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D09A9A9AD0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4D7DFE3D7EEF6D7EFF9D6F0FB8FB5D633628D1113138A8B
        8BC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BAC7CD7C9FB4497798237D9C0D
        799B036F9703638E1C638786A0B2C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0DFDFDFEDEDEDEFEFEFF0F0F0BABABA6969691212128A8A
        8AD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C7C7C7A1A1A17B7B7B7C7C7C76
        76766E6E6E646464656565A2A2A2D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4ADAFB0101314898C
        8EC8D0D4C8D0D4C8D0D4B4C6CE7BA6BC3B7EA0055F931E87B837A6D22B9BCA24
        96C71389BE007BBA35A8DD07477CA6B6C3C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0AFAFAF1212128C8C
        8CD0D0D0D0D0D0D0D0D0C6C6C6A7A7A7808080646464898989A6A6A69C9C9C97
        97978B8B8B7F7F7FAAAAAA4F4F4FB8B8B8D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4AEB0B11113178B8D
        91C8D0D4C8D0D4388EB0087DAB309FC659CCEB80F2FF7EF0FF77ECFF71EAFF63
        DBF44EC2E157C0DB67E7FF349FCB597E9AC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0B0B0B01313138D8D
        8DD0D0D0D0D0D08E8E8E7D7D7D9E9E9EC8C8C8E8E8E8E7E7E7E4E4E4E2E2E2D4
        D4D4BEBEBEBCBCBCDFDFDF9F9F9F828282D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D47E80821617199090
        91C8D0D4C8D0D45898A39FFFFF9AF8FF90F5FF8CF4FF89F4FF7BDAE76DB8C265
        A3AA5C8C9169C9DA6DE6FF5BD5F6245982C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D08080801717179090
        90D0D0D0D0D0D0939393F4F4F4EFEFEFECECECEBEBEBEBEBEBD3D3D3B2B2B29E
        9E9E888888C3C3C3DFDFDFD0D0D05F5F5FD0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D43030311515139593
        91C8D0D4C8D0D485A9B280D2DEA9F9FF89CFD770A3A874B1B87AC1C884DFEB89
        F6FF85F7FF80F3FF7AECFF7CF3FF065688B7C4CBC8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D03030301414149292
        92D0D0D0D0D0D0A7A7A7CCCCCCF1F1F1C9C9C99E9E9EACACACBABABAD8D8D8EC
        ECECECECECE9E9E9E4E4E4E9E9E95B5B5BC4C4C4D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4808383282E2EC0BF
        BEC8D0D4C8D0D4B9C8CC5DA4B0B3FCFFA9F7FFADFBFFA1FBFF94F8FF91F7FF85
        E0EC74B7BE68999D7EDBE889F6FF3190B5819FB2C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D08282822D2D2DBEBE
        BED0D0D0D0D0D0C7C7C79F9F9FF4F4F4F0F0F0F3F3F3F1F1F1EEEEEEEDEDEDD9
        D9D9B1B1B1949494D4D4D4ECECEC909090A1A1A1D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D3D5D5C8D0
        D4C8D0D4C8D0D4C8D0D4709FAAA8EFF8C1FBFF96D2D88DAFB28FB3B584B1B387
        C9D091EBF693F7FF8DF4FF8CF3FF5FCAE4497B96C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D4D4D4D0D0
        D0D0D0D0D0D0D0D0D0D09C9C9CE9E9E9F5F5F5CCCCCCACACACAFAFAFACACACC3
        C3C3E4E4E4EDEDEDEBEBEBEAEAEAC5C5C57D7D7DD0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4A3BCC37EC0C9D0FFFFB7EEF3AEF7FFB4FEFFC0FFFFBC
        FDFFB6FDFFA7E8F094C9CFA8F5FF93F5FF135F88C7CFD3C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0BBBBBBBBBBBBF9F9F9E9E9E9F1F1F1F5F5F5F7F7F7F6
        F6F6F5F5F5E2E2E2C4C4C4EFEFEFECECEC626262CFCFCFD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C5CDD1659AA5D3FFFFD7FEFFC4E6E9A0CBCE83ACB092
        BBBD97BDC0A0D3D8AFE3E9B7F8FFACFBFF2081A2A3B8C2C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0CDCDCD979797F9F9F9F9F9F9E3E3E3C6C6C6A8A8A8B6
        B6B6B9B9B9CECECEDEDEDEF2F2F2F3F3F37F7F7FB8B8B8D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D45B948AB2E2E7E4FFFFBFE7E9D7F8FBDBFFFFCA
        FFFFB9FEFFB0FDFFADF8FFAAF6FFAAF9FF53B5CD6B95A6C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D08A8A8ADEDEDEFBFBFBE3E3E3F5F5F5FAFAFAF8
        F8F8F6F6F6F4F4F4F1F1F1F0F0F0F1F1F1B1B1B1959595D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D484B7BDEEFFFFE8FFFFDBF1F3C8DDDEBF
        D3D4B5C6C5B5C9CAA9B8B9BAD8DAD1FDFF93EAF934728BC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0B2B2B2FDFDFDFCFCFCEFEFEFDADADAD1
        D1D1C3C3C3C7C7C7B6B6B6D5D5D5F8F8F8E4E4E4727272D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D477A1A7E9FFFFEEFAFAD8E4E2E5F6F5ED
        FFFFECFFFFE9FFFFE3FFFFD9FFFFCFFFFFC0FFFF10708EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D09E9E9EFCFCFCF8F8F8E2E2E2F3F3F3FC
        FCFCFCFCFCFCFCFCFBFBFBFAFAFAF9F9F9F7F7F76E6E6ED0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D495B0B6C2E6E7F9FFFFE5FDFECEEEEFB7
        DDE19FCDD289BFC678AFB768A2AB639CA66598A487ADB6C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0AEAEAEE2E2E2FEFEFEFAFAFAEAEAEAD9
        D9D9C9C9C9BABABAABABAB9E9E9E989898959595ABABABD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4B5C4C96C959F6C949D7497A082A2A98D
        AAB09AB1B8A6BAC0B2C2C7C0CBCFC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C3C3C3939393929292959595A0A0A0A8
        A8A8B0B0B0B9B9B9C1C1C1CACACAD0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      NumGlyphs = 2
    end
    object BtnDownload: TBitBtn
      Left = 290
      Top = 160
      Width = 220
      Height = 40
      Hint = 
        'Scarica sul LASER una o tutte le zone di mapping recuperandole d' +
        'al file'
      Caption = '*SCARICO RIFLETTORI SU TGV'
      TabOrder = 5
      OnClick = BtnDownloadClick
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000C40E0000C40E00000000000000000000C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4BACEE07A9CB6608CAA4966881B6CA71166A31B78B71D76B9127ED71680
        DAB9BCC2C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D1D1D19F9F9F8F8F8F6C6C6C7474746E6E6E8080807F7F7F8C8C8C8E8E
        8EBDBDBDD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        CDE7F82895E21191E81392E81C8FE20D91EA0A8FE5098FE60A8FE5088BE40B84
        DA607895BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE
        BEBEBEBEBEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        E9E9E99F9F9F9C9C9C9D9D9D9A9A9A9C9C9C9999999999999999999696968F8F
        8F7D7D7DBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE
        BEBEBEBEBED0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        CDEBFE24A5F70D9AF50C94EF0C95F00D8BDD0B88DA1683CD1180C70A83D20C7C
        CC08264227262327262327262327262327262327262327262327262327262327
        2623C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        EDEDEDAEAEAEA5A5A59F9F9FA0A0A09595959292928C8C8C8888888C8C8C8787
        872A2A2A25252525252525252525252525252525252525252525252525252525
        2525D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        CCEAFB2482BC0E65980D60950F83D10C38540B2B3C0B1B240B13190B2F570A34
        5E0B1E394F504FBEBEBEC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        EBEBEB8888886A6A6A6666668D8D8D3B3B3B2C2C2C1B1B1B1313133636363B3B
        3B2323234F4F4FBEBEBED0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        CDE7F71F38440A0C060B20291278BF060B0B070B07070807090A0B0915290408
        09081116262B2B272623272623272623272623272623272623272623C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        E8E8E83838380909092020208181810A0A0A0909090707070A0A0A1919190707
        071111112A2A2A252525252525252525252525252525252525252525D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        CAE4F41E323D060B060A1F281276BA03070A0104020104020405080710230407
        08070A0B404547E5E5E6C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        E5E5E53333330808081F1F1F7E7E7E0707070303030303030505051414140606
        06090909454545E5E5E5D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        BDDDF0132C3A07070709212E1276BA0107090003020104020406060612250306
        060B0E0E6F7173C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        DFDFDF2D2D2D0707072222227E7E7E0606060202020303030505051616160505
        050D0D0D717171D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        BCDBED0F2A370606060A21311274B8000606010402010402030804101E30352F
        2B0B100C717375C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        DCDCDC2A2A2A0606062323237D7D7D0505050303030303030606062121212E2E
        2E0E0E0E737373D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        B6D7EA0D27350406060A22321070B40006060406060308030209062834419381
        730C0E08757775C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D8D8D82828280505052424247979790505050505050505050707073636367E7E
        7E0B0B0B767676D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        B7D4E70F24340306060625351172B6000606030804020803050A061A2A395C53
        4C12140F777977C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D6D6D62626260505052626267B7B7B0505050606060505050808082C2C2C5151
        51121212787878D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        B6D2E40E223304060607263C1373BA000708040606040606050A060518290606
        060E0E0D777777C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D4D4D42424240505052929297D7D7D0606060505050505050808081A1A1A0606
        060D0D0D777777D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        B6D2E410263A0408090828401374BD040E14030C0D040A0B040C0D0A1B2C040A
        05545652ADADADC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D4D4D42929290707072B2B2B7E7E7E0E0E0E0B0B0B0909090B0B0B1E1E1E0707
        07545454ADADADD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        B5CCDC1F4C72122F4B0B52871283D5176A9A0F5A870E567B0E547C0C3255080D
        09B8BAB9C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        CECECE5252523434345959598E8E8E6E6E6E5E5E5E5858585757573838380B0B
        0BB9B9B9D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        BCBEBE202928234058125E921987DB1DC5F016B8EE1CBAEA1DB0E71D517A0C10
        0FB6B8B7C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        BDBDBD272727434343646464939393BEBEBEB5B5B5B6B6B6AFAFAF5757570F0F
        0FB7B7B7D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D474746D2525181C333F1B96D621CAEE1BBCF01EC5F425C0EF406E8D0E11
        11878889C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D07171712121213333339B9B9BC1C1C1B9B9B9C0C0C0BCBCBC7272721010
        10888888D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D48D8D8763645D408DAF3FC7F13FC3F43EC6F753BBE62A65880C0E
        0F898A89C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D08B8B8B6161618E8E8EC4C4C4C2C2C2C5C5C5BCBCBC6868680E0E
        0E898989D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4D7DFE3D7EEF6D7EFF9D6F0FB8FB5D633628D1113
        138A8B8BC8D0D4C8D0D4C8D0D4C8D0D4C8D0D4BAC7CD7C9FB4497798237D9C0D
        799B036F9703638E1C638786A0B2C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0DFDFDFEDEDEDEFEFEFF0F0F0BABABA6969691212
        128A8A8AD0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C7C7C7A1A1A17B7B7B7C7C7C76
        76766E6E6E646464656565A2A2A2D0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4ADAFB01013
        14898C8EC8D0D4C8D0D4B4C6CE7BA6BC3B7EA0055F931E87B837A6D22B9BCA24
        96C71389BE007BBA35A8DD07477CA6B6C3C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0AFAFAF1212
        128C8C8CD0D0D0D0D0D0C6C6C6A7A7A7808080646464898989A6A6A69C9C9C97
        97978B8B8B7F7F7FAAAAAA4F4F4FB8B8B8D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D47070FFC8D0D4C8D0D4C8D0D4C8D0D4AEB0B11113
        178B8D91C8D0D4388EB0087DAB309FC659CCEB80F2FF7EF0FF77ECFF71EAFF63
        DBF44EC2E157C0DB67E7FF349FCB597E9AC8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D09A9A9AD0D0D0D0D0D0D0D0D0D0D0D0B0B0B01313
        138D8D8DD0D0D08E8E8E7D7D7D9E9E9EC8C8C8E8E8E8E7E7E7E4E4E4E2E2E2D4
        D4D4BEBEBEBCBCBCDFDFDF9F9F9F828282D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D46060FF0000FF1010FFC8D0D4C8D0D4C8D0D47E80821617
        19909091C8D0D45898A39FFFFF9AF8FF90F5FF8CF4FF89F4FF7BDAE76DB8C265
        A3AA5C8C9169C9DA6DE6FF5BD5F6245982C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D08F8F8F4C4C4C575757D0D0D0D0D0D0D0D0D08080801717
        17909090D0D0D0939393F4F4F4EFEFEFECECECEBEBEBEBEBEBD3D3D3B2B2B29E
        9E9E888888C3C3C3DFDFDFD0D0D05F5F5FD0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D46060FF0000FF0000FF0000FF1010FFC8D0D4C8D0D43030311515
        13959391C8D0D485A9B280D2DEA9F9FF89CFD770A3A874B1B87AC1C884DFEB89
        F6FF85F7FF80F2FF7AECFF7CF3FF065688B7C4CBC8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D08F8F8F4C4C4C4C4C4C4C4C4C575757D0D0D0D0D0D03030301414
        14929292D0D0D0A7A7A7CCCCCCF1F1F1C9C9C99E9E9EACACACBABABAD8D8D8EC
        ECECECECECE8E8E8E4E4E4E9E9E95B5B5BC4C4C4D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D49F9FFF3838FF0000FF0000FF0000FF1818FF5050FFC8D0D4808383282E
        2EC0BFBEC8D0D4B9C8CC5DA4B0B3FCFFA9F7FFADFBFFA1FBFF94F8FF91F7FF85
        E0EC74B7BE68999D7EDBE889F6FF3190B5819FB2C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0BBBBBB7373734C4C4C4C4C4C4C4C4C5D5D5D848484D0D0D08282822D2D
        2DBEBEBED0D0D0C7C7C79F9F9FF4F4F4F0F0F0F3F3F3F1F1F1EEEEEEEDEDEDD9
        D9D9B1B1B1949494D4D4D4ECECEC909090A1A1A1D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40000FF0000FF0000FF6060FFC8D0D4C8D0D4C8D0D4D3D5
        D5C8D0D4C8D0D4C8D0D4709FAAA8EFF8C1FBFF96D2D88DAFB28FB3B584B1B387
        C9D091EBF693F7FF8DF4FF8CF3FF5FCAE4497B96C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D04C4C4C4C4C4C4C4C4C8F8F8FD0D0D0D0D0D0D0D0D0D4D4
        D4D0D0D0D0D0D0D0D0D09C9C9CE9E9E9F5F5F5CCCCCCACACACAFAFAFACACACC3
        C3C3E4E4E4EDEDEDEBEBEBEAEAEAC5C5C57D7D7DD0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D40808FF0000FF0000FF3838FFC8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4A3BCC37EC0C9D0FFFFB7EEF3AEF7FFB4FEFFC0FFFFBC
        FDFFB6FDFFA7E8F094C9CFA8F5FF93F5FF135F88C7CFD3C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D05151514C4C4C4C4C4C737373D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0BBBBBBBBBBBBF9F9F9E9E9E9F1F1F1F5F5F5F7F7F7F6
        F6F6F5F5F5E2E2E2C4C4C4EFEFEFECECEC626262CFCFCFD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D43030FF0000FF0000FF0000FF8F8FFFC8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C5CDD1659AA5D3FFFFD7FEFFC4E6E9A0CBCE83ACB092
        BBBD97BDC0A0D3D8AFE3E9B7F8FFACFBFF2081A2A3B8C2C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D06D6D6D4C4C4C4C4C4C4C4C4CB0B0B0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0CDCDCD979797F9F9F9F9F9F9E3E3E3C6C6C6A8A8A8B6
        B6B6B9B9B9CECECEDEDEDEF2F2F2F3F3F37F7F7FB8B8B8D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D49797FF0000FF0000FF0000FF0000FF3030FF4040FF4040
        FF4040FF4040FF4040FF4040FF5B948AB2E2E7E4FFFFBFE7E9D7F8FBDBFFFFCA
        FFFFB9FEFFB0FDFFADF8FFAAF6FFAAF9FF53B5CD6B95A6C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0B6B6B64C4C4C4C4C4C4C4C4C4C4C4C6D6D6D7979797979
        797979797979797979797979798A8A8ADEDEDEFBFBFBE3E3E3F5F5F5FAFAFAF8
        F8F8F6F6F6F4F4F4F1F1F1F0F0F0F1F1F1B1B1B1959595D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D44040FF0000FF0000FF0000FF0000FF0000FF0000
        FF0000FF0000FF0000FF0000FFC8D0D484B7BDEEFFFFE8FFFFDBF1F3C8DDDEBF
        D3D4B5C6C5B5C9CAA9B8B9BAD8DAD1FDFF93EAF934728BC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D07979794C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4CD0D0D0B2B2B2FDFDFDFCFCFCEFEFEFDADADAD1
        D1D1C3C3C3C7C7C7B6B6B6D5D5D5F8F8F8E4E4E4727272D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D44848FF0000FF0000FF0000FF0000FF0000
        FF0000FF0000FF0000FF0000FFC8D0D477A1A7E9FFFFEEFAFAD8E4E2E5F6F5ED
        FFFFECFFFFE9FFFFE3FFFFD9FFFFCFFFFFC0FFFF10708EC8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D07E7E7E4C4C4C4C4C4C4C4C4C4C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4CD0D0D09E9E9EFCFCFCF8F8F8E2E2E2F3F3F3FC
        FCFCFCFCFCFCFCFCFBFBFBFAFAFAF9F9F9F7F7F76E6E6ED0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D49F9FFF4040FF0808FF0000FF0000
        FF0000FF0000FF0000FF0000FFC8D0D495B0B6C2E6E7F9FFFFE5FDFECEEEEFB7
        DDE19FCDD289BFC678AFB768A2AB639CA66598A487ADB6C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0BBBBBB7979795151514C4C4C4C4C
        4C4C4C4C4C4C4C4C4C4C4C4C4CD0D0D0AEAEAEE2E2E2FEFEFEFAFAFAEAEAEAD9
        D9D9C9C9C9BABABAABABAB9E9E9E989898959595ABABABD0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4B5C4C96C949E6C949E7497A082A2A98D
        AAB09AB1B8A6BAC0B2C2C7C0CBCFC8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0C3C3C3929292929292959595A0A0A0A8
        A8A8B0B0B0B9B9B9C1C1C1CACACAD0D0D0D0D0D0D0D0D0D0D0D0C8D0D4C8D0D4
        C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0
        D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8
        D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0
        D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0D0}
      NumGlyphs = 2
    end
    object BtnChangeReflector: TBitBtn
      Left = 10
      Top = 210
      Width = 220
      Height = 40
      Caption = '*DELETE RIFLETTORI DA ZONA'
      TabOrder = 6
      OnClick = BtnChangeReflectorClick
      Glyph.Data = {
        36180000424D3618000000000000360000002800000040000000200000000100
        18000000000000180000C40E0000C40E00000000000000000000FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF8C8CA80505928C8CA9FF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FF8C8CAC0808A58C8CACFF00FF696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        696969696969696969696969696969696969699494942F2F2F94949469696969
        6969696969696969696969696969959595363636959595696969FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FF8C8CA604048E2222B30404998C8CA9FF
        00FFFF00FFFF00FFFF00FF8C8CAC0707AB2A2AC60707A78C8CAC696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        696969696969696969696969696969699393932D2D2D4D4D4D30303094949469
        6969696969696969696969959595383838585858363636959595FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FF0505881E1EAE0B0BB12929C405059D8C
        8CA9FF00FFFF00FF8C8CAB0808AE3434D71616C72A2AC30707A4696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        696969696969696969696969696969692C2C2C4949493C3C3C57575732323294
        94946969696969699595953939396464644A4A4A575757353535FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FF8C8CA502028C2626BC1313C23131D406
        06A18C8CA98C8CA90A0AAE3C3CE82020D93131D30606A98C8CAC696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        696969696969696969696969696969699393932B2B2B52525247474761616134
        34349494949494943B3B3B6F6F6F575757616161363636959595FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF8C8CA503038F2C2CCA1C1CD139
        39E20707A40808AA4444FA2828EA3838E00606AB8C8CACFF00FF696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        696969696969696969696969696969696969699393932C2C2C5B5B5B5252526B
        6B6B3535353838387A7A7A6262626A6A6A373737959595696969FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFA7AAAB8398
        A4668599528EA1398CA32489A31389A80F86A8218DB13D7EAE0505943232D522
        22DD3C3CEB4242F32D2DF23B3BE90707A98C8CABFF00FFFF00FF696969696969
        696969696969696969696969696969696969696969696969696969A9A9A99999
        998787878C8C8C8989898585858484848282828B8B8B8484842F2F2F62626259
        59597070707676766767676F6F6F373737959595696969696969FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF93A3AA5D859B2A6A8E045A910066
        A61680BA2398CC26AAE024B5EC14ABE8009EE103A9E526B3E73D96D205059633
        33D92222E02525E63A3AE70505A58C8CA9FF00FFFF00FFFF00FF696969696969
        696969696969696969696969696969696969A3A3A38787876D6D6D6060606D6D
        6D8585859A9A9AABABABB4B4B4ACACACA0A0A0A8A8A8B2B2B29D9D9D30303064
        64645A5A5A5E5E5E6D6D6D343434949494696969696969696969FF00FFFF00FF
        FF00FFFF00FFA7AAAD7B98A6497F9B176795006BA42196C944C1EF60DCFC74EC
        FF70E9FF6AE4FE68E3FD67E3FD69E3FD6AE1FC51C0E94BAFDD3D82C312129839
        39D31C1CD41F1FD93535DD0505A08C8CA9FF00FFFF00FFFF00FF696969696969
        696969696969AAAAAA9898988181816B6B6B6F6F6F979797C0C0C0D7D7D7E4E4
        E4E1E1E1DDDDDDDCDCDCDCDCDCDCDCDCDBDBDBBFBFBFB1B1B18D8D8D3A3A3A67
        67675353535656566767673333339494946969696969696969699FA8AD7498A9
        538BA329779B056EA2158EC03BB6E161DCFE82F1FF7FF0FF79ECFF74E9FC71E6
        FC6FE5FC6DE4FC6BE3FC69E2FB67E1FB66E1FC79E6FE76BEE31617915F5FD24A
        4AD04545D23030CF1717CA2D2DCC04049D8C8CA9FF00FFFF00FFA8A8A8989898
        8B8B8B7878787171718F8F8FB4B4B4D8D8D8E8E8E8E7E7E7E4E4E4E1E1E1DFDF
        DFDEDEDEDDDDDDDCDCDCDBDBDBDADADADBDBDBE0E0E0C0C0C03B3B3B81818172
        72726F6F6F5F5F5F4C4C4C5C5C5C3131319494946969696969693887A032ACD3
        4CC1E46BE1FD8BF4FF91F6FF8CF3FF89F0FF85EFFE81EDFE7FECFD7CEAFD79EA
        FD77EAFD75E8FE74E8FE72EDFF72EDFE80E5F972A6C71A1A8B6B6BCB5656C763
        63D11515961313984646CC1E1EBF2A2ABE0303988C8CA9FF00FF858585A9A9A9
        BEBEBEDBDBDBEBEBEBEDEDEDEAEAEAE8E8E8E7E7E7E5E5E5E4E4E4E3E3E3E2E2
        E2E2E2E2E1E1E1E1E1E1E4E4E4E4E4E4DFDFDFA9A9A93B3B3B87878777777783
        83833B3B3B3A3A3A6E6E6E4E4E4E5656562F2F2F94949469696969949894EFFA
        96F6FF97F3FF92F2FE8DF0FE8BF0FE8AEFFE89EFFE89F0FE87F0FF87F2FF86F4
        FF84F5FF7BE6F775D6E65C868C657E7F676B802020897A7ACB6262C26F6FCB1A
        1B9176BBE27ABFE51515975555C73737BA4242BA0A0A978C8CA9909090E7E7E7
        EDEDEDECECECEAEAEAE8E8E8E8E8E8E7E7E7E7E7E7E8E8E8E8E8E8E9E9E9EAEA
        EAEBEBEBDEDEDECFCFCF8383837B7B7B7070703F3F3F9292927E7E7E8A8A8A3E
        3E3EBEBEBEC2C2C23B3B3B7777775E5E5E65656534343494949496A7A970B6BF
        97F5FFA2F4FE9DF2FE97F2FE91F2FF90F6FF90F7FF90F9FF88E8F475BBC25C78
        78586E6D515A5760878972BEC892EDFB1E21868F8FD38383CE7F7FCC1E1F8D7C
        BDE284E8FB85ECFE558FC51313945353BB3535AF4444B10C0C93A5A5A5B0B0B0
        EDEDEDEDEDEDEBEBEBEBEBEBEAEAEAEDEDEDEDEDEDEEEEEEE0E0E0B5B5B57474
        746B6B6B585858838383B8B8B8E6E6E63E3E3EA3A3A39999999696963F3F3FC0
        C0C0E2E2E2E5E5E5989898393939727272595959646464343434AEAEAE6C9A9F
        95F0FDA4F4FFA8F5FF9CF2FF9FEFFA6A90925C78785F7A7B607C7D77B5BD83D9
        E38BF3FF8CF7FF8AF6FF89F3FF92F0FE86C2E225278A9595D424258B82C0E18A
        ECFC78E8FC78E9FE6DD3F14365961515925858B91212928C8CA8AEAEAE969696
        E9E9E9EEEEEEEFEFEFECECECE9E9E98C8C8C747474777777797979B0B0B0D2D2
        D2EAEAEAEDEDEDECECECEAEAEAE9E9E9C4C4C4444444A7A7A7434343C2C2C2E5
        E5E5E1E1E1E2E2E2D0D0D06F6F6F3A3A3A757575383838949494FF00FF90A6A8
        7BC1CAA1F7FFB0F5FFADF7FF8AD2DB91C6CB96D7E098F8FF93F8FF92F4FF90F2
        FF8FF1FF8DF0FE8CF0FE8CEFFE8AF0FE99F3FE89C6E21F228789C7E293F0FE83
        EEFE7FEEFE7EEDFF82F2FE22709E7681A112128C8C8CA6FF00FF696969A4A4A4
        BBBBBBEFEFEFF0F0F0F0F0F0CCCCCCC1C1C1D2D2D2EFEFEFEEEEEEECECECEAEA
        EAEAEAEAE8E8E8E8E8E8E8E8E8E8E8E8ECECECC7C7C73F3F3FC8C8C8E9E9E9E6
        E6E6E6E6E6E5E5E5E8E8E8747474898989363636939393696969FF00FFACAEAE
        68989C9DF7FFB4F7FEB4F6FFAFF7FF9EF6FFAFF7FFADF6FFA7F5FE9AF3FF93F3
        FF93F5FF92F7FF92F8FF92F9FF8AE7F471AAB07BA7AD707E7B626B695A6F6F86
        EEFE86EEFE85EEFE89F5FF3098C2587E93FF00FFFF00FFFF00FF696969ADADAD
        939393EFEFEFF1F1F1F1F1F1F1F1F1EEEEEEF1F1F1F0F0F0EEEEEEECECECEBEB
        EBECECECEDEDEDEEEEEEEFEFEFE0E0E0A5A5A5A3A3A37B7B7B6969696C6C6CE6
        E6E6E6E6E6E6E6E6EBEBEB9898987F7F7F696969696969696969FF00FFFF00FF
        8BA2A686CDD5AFF8FFBDF7FFBAF7FFB1F7FE9FF6FFB7FEFFB5FAFFB2F8FF9EDA
        E17BA7AC6785875C757353615F68919378B8C082D3DD8AEBF890F9FF8BF2FF8B
        F1FF89EFFE87EFFE89F0FF62D4F112567DAEAEAEFF00FFFF00FF696969696969
        A0A0A0C7C7C7F1F1F1F2F2F2F2F2F2F1F1F1EEEEEEF6F6F6F3F3F3F2F2F2D5D5
        D5A3A3A38282827171715E5E5E8C8C8CB3B3B3CCCCCCE3E3E3EEEEEEEAEAEAE9
        E9E9E7E7E7E7E7E7E8E8E8CFCFCF595959AEAEAE696969696969FF00FFFF00FF
        A9ACAD6D9EA2AAFBFFC3F9FEC0F8FFC0FBFF9BCACF5A6C686A74707D8B8A8094
        92A5DBE0A6E0E7B0F7FFA9F4FEA7FAFFA0F6FF9AF5FF96F2FF93F1FE91F0FE8F
        F0FE8DF0FE8BF0FE8AEFFE8FF7FF00608F889BA2FF00FFFF00FF696969696969
        ABABAB999999F2F2F2F4F4F4F3F3F3F5F5F5C6C6C66868687171718989899191
        91D6D6D6DBDBDBF1F1F1EEEEEEF2F2F2EEEEEEEDEDEDEBEBEBEAEAEAE9E9E9E9
        E9E9E8E8E8E8E8E8E7E7E7EDEDED6363639A9A9A696969696969FF00FFFF00FF
        FF00FF85A0A396DBE1C1FAFFCAF9FFC8FBFFB0D4D7B2E3E8A2FAFFABFCFFBEFE
        FFBDFAFFBAF9FFB7F7FFB4F6FFB3F7FFB1F5FFB0F6FFAEF6FFAEF8FFACFAFFAA
        FBFFA5F6FFA3F4FE9EF3FE91F7FF319AC2517A8FFF00FFFF00FF696969696969
        6969699D9D9DD4D4D4F4F4F4F5F5F5F6F6F6D0D0D0DEDEDEF1F1F1F3F3F3F7F7
        F7F4F4F4F3F3F3F2F2F2F1F1F1F1F1F1F0F0F0F0F0F0F0F0F0F1F1F1F2F2F2F2
        F2F2EFEFEFEDEDEDECECECEDEDED9999997B7B7B696969696969FF00FFFF00FF
        FF00FFA6ABAC78AAAEBBFEFFD1FAFFCDFAFFCCFBFFCBFBFFC7F9FEAEF6FEA0F4
        FEAEF6FFBFF9FFC2FCFFBFFDFFBDFEFFBBFDFFB6F7FFAAE5EC8CAEB285A4A776
        878793C3C9AAF6FFA9F5FF96F3FF68D6F11A6080FF00FFFF00FF696969696969
        696969AAAAAAA5A5A5F6F6F6F6F6F6F6F6F6F6F6F6F6F6F6F4F4F4F0F0F0EDED
        EDF0F0F0F4F4F4F6F6F6F6F6F6F6F6F6F6F6F6F1F1F1E0E0E0ABABABA1A1A185
        8585BFBFBFF0F0F0EFEFEFEBEBEBD1D1D1616161696969696969FF00FFFF00FF
        FF00FFFF00FF819EA0A8EBF0D3FDFFD6FBFFD3FCFFD2FCFFD3FFFFD4FFFFC5FC
        FFA2E6EC94DDE584A6A98599998FA1A081888879807E98B6B79FC7CAA5D5DBB5
        F6FEB5FAFFB2F5FFB1F5FFA2F3FE98F8FF00648E91A0A6FF00FF696969696969
        6969696969699B9B9BE4E4E4F8F8F8F7F7F7F8F8F8F8F8F8F9F9F9FAFAFAF6F6
        F6E0E0E0D7D7D7A3A3A39696969E9E9E8787877E7E7EB2B2B2C3C3C3D1D1D1F0
        F0F0F3F3F3F0F0F0F0F0F0EDEDEDEFEFEF656565A0A0A0696969FF00FFFF00FF
        FF00FFFF00FFA1ABAC89B9BCCCFFFFDDFDFFD9FDFFD1F2F4A0ADAC8F96948F95
        939DA9A797A7A795BDBE93D3D9A5FBFFAEFCFFB7FDFFBDFCFFC1FBFFC2F9FFBF
        F9FFBCF8FFBAF6FFB8F7FFB5F6FE9CF9FF329EC0598295FF00FF696969696969
        696969696969AAAAAAB4B4B4F9F9F9F9F9F9F9F9F9EEEEEEABABAB9494949393
        93A7A7A7A5A5A5B8B8B8CDCDCDF2F2F2F4F4F4F5F5F5F5F5F5F5F5F5F4F4F4F4
        F4F4F3F3F3F1F1F1F2F2F2F0F0F0F0F0F09B9B9B838383696969FF00FFFF00FF
        FF00FFFF00FFFF00FF7E9D9EBDF9FDE4FDFFDFFEFFDFFEFFD8F6F8DFFFFFDDFF
        FFDBFFFFD9FFFFD7FEFFD4FCFFC6FAFEB8F7FEACF6FEA2F5FE9EF4FFA1F6FFA6
        F7FFA9FAFFA8F8FFA8F4FEA6F4FE9CF4FF6ED9F1216783FF00FF696969696969
        696969696969696969999999F3F3F3FAFAFAFAFAFAFAFAFAF3F3F3FBFBFBFBFB
        FBFAFAFAFAFAFAF9F9F9F8F8F8F5F5F5F1F1F1EFEFEFEEEEEEEDEDEDEFEFEFF0
        F0F0F2F2F2F0F0F0EEEEEEEEEEEEEDEDEDD3D3D3676767696969FF00FFFF00FF
        FF00FFFF00FFFF00FF98A7A89FCDCFDCFFFFE7FDFFE5FDFFE2FEFFE2FEFFDFFC
        FFDDFDFFDCFEFFDCFDFFDAFEFFDAFFFFDAFFFFDAFFFFD7FFFFD0FFFFAED5D8A0
        C5C886A2A28CB1B3B7F8FFB5F6FEB0F5FEA1FAFF00688E98A3A8696969696969
        696969696969696969A5A5A5C8C8C8FBFBFBFBFBFBFAFAFAFBFBFBFBFBFBF9F9
        F9F9F9F9FAFAFAF9F9F9FAFAFAFAFAFAFAFAFAFAFAFAFAFAFAF9F9F9D1D1D1C1
        C1C19E9E9EADADADF2F2F2F0F0F0EFEFEFF1F1F1676767A3A3A3FF00FFFF00FF
        FF00FFFF00FFFF00FFADAEAE82A1A1D2FFFFEDFFFFE9FEFFE7FDFFE8FFFFE7FF
        FFE5FFFFDEF7F8C3D6D6B7C7C6B6C7C6989D9A929895A2ADAC979F9D9EA7A7B5
        CECEBFDCDFCEF7FCCEFBFFCCF9FFCAFAFEA7FAFF38A1BF618694696969696969
        696969696969696969ADADAD9D9D9DF9F9F9FCFCFCFBFBFBFBFBFBFCFCFCFCFC
        FCFCFCFCF4F4F4D3D3D3C4C4C4C4C4C49B9B9B969696ABABAB9D9D9DA5A5A5CB
        CBCBD9D9D9F3F3F3F7F7F7F5F5F5F5F5F5F2F2F29E9E9E858585FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FF91A1A1B7E4E5EBFFFFEDFEFFEEFFFFD2DEDDA4A6
        A3B8C0BFAEB4B2C7D4D4D1E4E4D9F0F1E1FBFDE4FFFFE1FFFFE0FFFFDEFFFFDC
        FFFFD9FDFFD7FCFFD5FCFFD3FAFFD3FBFFBBFAFF74D9EE296981696969696969
        6969696969696969696969699F9F9FDFDFDFFCFCFCFCFCFCFDFDFDDCDCDCA4A4
        A4BEBEBEB2B2B2D2D2D2E1E1E1EDEDEDF8F8F8FBFBFBFBFBFBFBFBFBFBFBFBFB
        FBFBF9F9F9F8F8F8F8F8F8F7F7F7F7F7F7F4F4F4D3D3D3686868FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFA6ABAB97B8B8E3FFFFF4FFFFF0FFFFF1FFFFF2FF
        FFF1FFFFF0FFFFEEFFFFEBFFFFEAFEFFE9FEFFE7FDFFE5FEFFE6FFFFE4FEFFDF
        FDFFD8FFFFD2FEFFCBFFFFC5FEFFBEFEFFB6FDFFB2FFFF0A6C89696969696969
        696969696969696969696969AAAAAAB4B4B4FBFBFBFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFCFCFCFCFCFCFBFBFBFBFBFBFBFBFBFCFCFCFBFBFBFA
        FAFAFAFAFAF9F9F9F9F9F9F7F7F7F7F7F7F5F5F5F6F6F6696969FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FF8BA0A0D4FEFEF7FFFFF6FFFFF5FFFFF5FF
        FFF5FFFFF3FFFFEEFFFFE7FFFFE3FFFFDBFFFFD7FFFFCEFFFFC1F8FCB0EAEDA4
        DDE29BD3D791C6CA87BBC07FB0B47BA5AB7AA2A67A9DA190A6A7696969696969
        6969696969696969696969696969699D9D9DF9F9F9FEFEFEFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFCFCFCFBFBFBFAFAFAFAFAFAF9F9F9F2F2F2E4E4E4D7
        D7D7CDCDCDC1C1C1B6B6B6ABABABA2A2A29E9E9E9A9A9AA3A3A3FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FF99A6A6B8DDDDE7FFFFE9FFFFDDFDFDCDF0
        F0C1E6E6B1D8D8A8CCCE9BC2C291B6B586AAAC87A6A883A0A1879F9F8CA0A193
        A5A597A6A79FA9A9A3AAAAAAADADADAEAEFF00FFFF00FFFF00FF696969696969
        696969696969696969696969696969A4A4A4D8D8D8FCFCFCFCFCFCF9F9F9ECEC
        ECE1E1E1D3D3D3C8C8C8BDBDBDB1B1B1A6A6A6A3A3A39C9C9C9C9C9C9E9E9EA2
        A2A2A4A4A4A7A7A7A9A9A9ACACACADADAD696969696969696969FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFA9AEAE8EA8A892ADAD8BA2A28DA1A194A4
        A498A4A49DA8A8A1A9A9A5AAAAA8ACACAEAEAEFF00FFFF00FFFF00FFFF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF696969696969
        696969696969696969696969696969ADADADA5A5A5A9A9A99F9F9F9E9E9EA2A2
        A2A2A2A2A6A6A6A8A8A8A9A9A9ABABABAEAEAE69696969696969696969696969
        6969696969696969696969696969696969696969696969696969FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        6969696969696969696969696969696969696969696969696969FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        6969696969696969696969696969696969696969696969696969FF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        6969696969696969696969696969696969696969696969696969696969696969
        6969696969696969696969696969696969696969696969696969}
      NumGlyphs = 2
    end
    object BtnSave: TBitBtn
      Left = 290
      Top = 210
      Width = 220
      Height = 40
      Hint = 
        'Memorizza eventuali modifiche sui dati sul file Mappaxx.dat spec' +
        'ifico del TGV'
      Caption = '*SALVA'
      TabOrder = 7
      OnClick = BtnSaveClick
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
      NumGlyphs = 2
    end
  end
  object TextStato: TStaticText
    Left = 0
    Top = 539
    Width = 766
    Height = 17
    Align = alBottom
    AutoSize = False
    BorderStyle = sbsSunken
    Caption = 'TextStato'
    TabOrder = 3
  end
  object GroupBox4: TGroupBox
    Left = 5
    Top = 220
    Width = 226
    Height = 155
    Caption = '*Dati aggiuntivi'
    TabOrder = 4
    object Label5: TLabel
      Left = 10
      Top = 20
      Width = 137
      Height = 13
      Caption = '*Minimo di riflettori da leggere'
    end
    object Label6: TLabel
      Left = 10
      Top = 41
      Width = 151
      Height = 13
      Caption = '*Numero totale di zone mappate'
    end
    object Label11: TLabel
      Left = 11
      Top = 65
      Width = 169
      Height = 13
      Caption = '*Numero rotaz. per (media Mapping)'
    end
    object Label17: TLabel
      Left = 11
      Top = 88
      Width = 129
      Height = 13
      Caption = '*G Minimo (media Mapping)'
    end
    object Label18: TLabel
      Left = 12
      Top = 132
      Width = 158
      Height = 13
      Caption = '*Rotazione Testa  Laser ORARIA'
    end
    object Label19: TLabel
      Left = 11
      Top = 111
      Width = 130
      Height = 13
      Caption = '*Raggio riflettori (default 50)'
    end
    object EditMinR: TEdit
      Left = 183
      Top = 15
      Width = 31
      Height = 21
      Hint = 'Numero minimo di riflettori da leggere durante la navigazione'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = 'EditRot'
    end
    object EditMaxZone: TEdit
      Left = 183
      Top = 38
      Width = 31
      Height = 21
      Hint = 'Numero totale di zone mappate'
      Color = clBtnFace
      Enabled = False
      MaxLength = 2
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 1
      Text = 'EditRot'
    end
    object EditNumScanMediaMap: TEdit
      Left = 183
      Top = 62
      Width = 31
      Height = 21
      Hint = 'Numero minimo di riflettori da leggere durante la navigazione'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      Text = 'Edit'
    end
    object EditGMediaMap: TEdit
      Left = 183
      Top = 85
      Width = 31
      Height = 21
      Hint = 'Numero minimo di riflettori da leggere durante la navigazione'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      Text = 'Edit'
    end
    object CheckBoxRotTesta: TCheckBox
      Left = 201
      Top = 132
      Width = 15
      Height = 17
      TabOrder = 4
    end
    object EditRaggioRif: TEdit
      Left = 183
      Top = 108
      Width = 31
      Height = 21
      Hint = 'Numero minimo di riflettori da leggere durante la navigazione'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
      Text = 'Edit'
    end
  end
  object BtnCalEncoder: TButton
    Left = 254
    Top = 490
    Width = 220
    Height = 40
    Caption = '*Calibrazione Encoder'
    TabOrder = 5
    Visible = False
    OnClick = BtnCalEncoderClick
  end
  object BtnCalSterzo: TButton
    Left = 534
    Top = 490
    Width = 220
    Height = 40
    Caption = '*Calibrazione Sterzo'
    TabOrder = 6
    Visible = False
    OnClick = BtnCalSterzoClick
  end
  object pnlChangeTGV: TPanel
    Left = 254
    Top = 10
    Width = 105
    Height = 49
    BevelInner = bvLowered
    Ctl3D = False
    ParentCtl3D = False
    TabOrder = 7
    object imgTgv: TImage
      Left = 4
      Top = 4
      Width = 42
      Height = 42
      Align = alCustom
      Center = True
      Picture.Data = {
        07544269746D6170F6120000424DF61200000000000036000000280000002800
        0000280000000100180000000000C0120000120B0000120B0000000000000000
        0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        7D8B940A19250C1B2F074E93073C6F224D764B7597818789C3CCD1F0EEEDFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF90A0AA0E232E191E211654
        8F1556A11055A40E53A4064FA4013B7B0B5096234F7159708397AFC1D2D1CEFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFF1F2F37171F20162A3B145FA511549E114D9412
        4E99164E9B1753A51553A60C53A50746920A44820F3B643B6D9C707D8AB2BAC3
        E6E5E8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFF1C2224131A1A102C481567B113539F144D9B144E99144D97144D97
        134C980D4EA11D488A6C3033493D5B084FA701428B0749931D4E824C688585A2
        B7C2C6C7F4F4F3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF151C22
        1419180D3A5F166AB612539E1150981151A01352A31550A1154E9C0F4E9C1F46
        83783029633540124D9B17519F154FA31050A2064DA20A4186153E702A629263
        7B8EA0B3C2D7D7D4FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF172F36192E351351821368
        B31457A51030540E1F3111335C124079144B8F1451A113529F28497E2C437810
        4D9B124C961249961449980E4EA6473E60703437164B91004BA00649990B3F7C
        8FA9C2FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFF1328310A1920124D7E156AB81356A91026440D
        0A030D0F0B0B0E110B15210F223B1232590F4E9B104FA5134DA11052A51250A3
        154DA0084FA23741697C312A214B810F52A41B55AC0348A6607A9CFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFF1191AE1282AC1467A01469B41456A80F243F0D0F0D0E15160E1313
        0F10110D0D0E0D0B0912417E1250A413396E0E2947113B6E144585104D9A1353
        A225519517509F0F52A01653A0034598566F8EFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF2A93A8
        03ABD21281B81266B11557A70F21390D0F0C0E14140E13120E12140E12180E11
        171344821255A8102F550A0900080D0C091113091A250C263D0C336213407E14
        519A1054A61353A2A6B8CDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF90979B376A932E7FBA1164
        B11457A60C20320D0F0D0E14130D12100D11120E11170B0F12133F80145AB40F
        32590C0E090A0F160B0E120B0F0B0A0D070C0F090D09040740681087CE2E4862
        524F4E787979AAAAAAD8D8D8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF91A1BC3C7AB50F64B21557A40B1D2B0C
        0F0E0E14130D12120D11130D11160B101111427E1159B51032580C0D070D1116
        0D10140D12120E14141117180506043C596ACCE1EC0F0E0C21262B4B4A4C2827
        2A2323233232325454548A8A8AC0C0C0EFEFEFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFF8DA3C03B81BA1065B21657A30C1A240A100E0E13130C0F14
        0B0E140B0D120A1117114D91145BB61131570B0C05090D12070A0F090E0D0B10
        0F0F1412040809504F53C5C2C20A12145E6260FFFFFFEAEAEACCCCCC9C9C9C6C
        6C6C4646462F2F2F333333616161A3A3A3DDDDDDFFFFFFFFFFFFFFFFFFF2F4F5
        869EB83380B90E65B41557A00B17200B110F0E14130B0E130A0D120A0C100810
        17104D94155DB71131580A0907070B11070A0E090E0D090E0D080D0C0F141400
        020551535825292C424548FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBEBEB
        C4C4C49696966D6D6D646464757574FFFFFFFFFFFFEFF2F394A7C03383BC0B66
        B514569D09141B0B100F0F13130C0F13090C11090B0F08111A115099165EB810
        3058070506070B11070A0F090E0D090E0D090E0E0C100F12161635383C101318
        3E4145FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFE3E3E2FFFFFFFFFFFFF0F2F3A2B7CB2F84C00D64B915549A0A13160B
        0E120B0E130B0E13090C12090B0E09121C15529D1361BB0D3058080706070B11
        070A0E080C0C090E0D090D0D0B100E0D121200000603070920242389898BAFAE
        B1DBDBDCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFE8ECEE9FB8CD2D84C10C64BA1551960A11120A0E120A0D12090C11
        090C120A0B0E0A12211552A1165CB60E3058090807080C12070A0F090D0E090D
        0E090D0E0C101205090A4344482C2F301B201EA8A8AB8E8C92817F85939394A0
        A0A0C5C6C1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDDE5E9
        95B1C92681C00F66BD144D90090E0E0B10130C0F14090C11090C120A0C0F0B17
        221458A51660BD102F57090707090D130A0D120A0D12090C11090C110C0F1500
        0000C6C5C4656867121716FFFFFFFFFFFFFFFFFFFFFFFFE5E5E5E6E7E4FFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDBE4E899B6CE1E7DC01169
        BF154B870B0C0D0C10150C0F14090C11090C120A0B0E0B1926155BAB1766C010
        30560A09090B0F15090C110A0D12090C110A0D120C0F1407090D85878935373B
        111518E9E9EAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFE3E5E9A4BBD41A7BC0106BBF12477F0C0A0C0C
        11170B0F150B0F140C0F140C0D0F0B1928155DAF1767C31030560B0A090C1016
        0B0E13090C110A0D120C0F140F1217020207B1B2B4696B6F04060BE3E3E4FFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFE8E5E7A3C2D41178BD146DC01143760E0B0D0B0E0D080F100C1016
        0C10170D0E110D1A2E145AB3126AC30E2E540D0B0B0E12190C0E140C0F140C0F
        140C0F140F1217000001ABADAB7B7C7E000003DBDBDCFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBE8E9
        99C2D60A78C21471CA123D6C0C07041531510C13180B0A050D0D090B0C0A0E16
        211658AC146BC70D30540C0C0A0C10160A12160C10160C10160C10160C10160D
        11171315170C0F1104070AC7C8C8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEEEBEB93BCD80767AD165E
        A7114E8D0F3A6D13509C0F376E0A2C500B29470C1D2B0C253C1264B3156FCE0B
        30550C161F1330510809050D0B0C0D0B0B0C0B0B0E0D0C0E11181C1E2A3D3E43
        1F2227C9C9CAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFEFF2F56A707A191F2622252A212833232D421D
        2C412135521E375C16457F1368C11366B71267C01166C1125CA712539A1157A0
        0D3E700F37640E31560C2B4B0C233D0E1D32C4C4C7DDDCDDE2E2E5FFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFB6B9BE3535342D2C2A201E1C131312121014151414151511191312
        1C1414144B861375CB125BBA1245B30C8BCA0893CC0B8EC51080CA1074CE1175
        CE1073CE0C70C9156DB8DAE2EAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF5E5A61
        1416153C3C3C453C3A2D292712151811141513151511151C12151A1C1D22165F
        A11056C00F44B10594CA0B8EC40A93C6107FB92367AE1A6CB21D6FB81674BE14
        73BCC2CFDBFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF5451572B29292421224941
        405549484D443D342D2C1A1A1C1114160F13171512131A26351064B5116BC008
        90C70A91C20794C61B80AD62697E676D7D7479807D7D7F3574A4B8D0E2FFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFF8382864F484A3E37383F3A3B5953535048455B
        524C5B4F484D403B3B312E2A2426261F191E3B550881D00195D00298D00094D0
        0286C60D77BF1777BB1C74B51D6FAF0E6EB6C9E0EEFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFA29FA0696465534A4D5B5355605A5B565050574F4F5E5450
        635754635351635451533F3C3291B526C7F823BDED2AC3EF2ABFEA27BBED1FB6
        EE2ABAF528B8F21FABEAC8DFECFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFE4E3E3A39FA57470776A676A6B6669686366615D63615D635C585F6260
        676D65687D9FAD7BEAFF67DBFD39BAE929C6F633CBF74FCFF426AAD22FBCE226
        8CADBFC0C4FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFEFEFF0DFE1E3D3D4D8D0D1D5BDBCC0BABBBFD0D3D7D3DDE4EEF7FBFFFFFFBD
        CAE06585B9C3CBE5325778587084DFE4F0183E980E2958000105B0B1B3FFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD1DAED9EAFD8F9F9F3
        1D1E214B484BFFFFFFDAE3F7343946020204ABAFB1FFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEAEDEE272A334B4D56FFFF
        FFFFFFFF424447000003A2A7A8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFEBEBEC2C2A304B4E53FFFFFFFFFFFF47494D00
        00009A9F9EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFF2A272D4D4F54FFFFFFFFFFFF515357000000909392FFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEEEEEE969698
        2A282E525156FFFFFFFFFFFF64666B040507878786FFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD6D6D536353A46444A413F454947
        4C45444624272C0A0B14868685FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFF4F4F4A5A4A6110F1536343A9494963737370F121122
        2328656565FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFE8E8E75554553B393EFFFFFF9C9A9C302E320B0A0F2F2E30FFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFF}
      Proportional = True
      Stretch = True
      Transparent = True
    end
    object tgv_color: TShape
      Left = 88
      Top = 3
      Width = 14
      Height = 14
      Brush.Color = clSilver
    end
    object btnTgvUP: TBitBtn
      Tag = 1
      Left = 74
      Top = 18
      Width = 28
      Height = 28
      TabOrder = 0
      OnClick = btnTgvUpDnClick
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
      Layout = blGlyphTop
      NumGlyphs = 2
      Spacing = -1
    end
    object btnTgvDN: TBitBtn
      Left = 46
      Top = 18
      Width = 28
      Height = 28
      TabOrder = 1
      OnClick = btnTgvUpDnClick
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
      Layout = blGlyphBottom
      NumGlyphs = 2
      Spacing = -1
    end
    object txtTgv: TStaticText
      Left = 46
      Top = 3
      Width = 40
      Height = 14
      Alignment = taCenter
      AutoSize = False
      Caption = 'Tgv:00'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
    end
  end
  object TimerMapping: TTimer
    Enabled = False
    Interval = 500
    OnTimer = TimerMappingTimer
    Left = 260
    Top = 95
  end
end
