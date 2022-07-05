object FormSetCom: TFormSetCom
  Left = 169
  Top = 160
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Inizializza Seriali'
  ClientHeight = 145
  ClientWidth = 441
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
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object BoxHostLink: TGroupBox
    Left = 5
    Top = 5
    Width = 431
    Height = 136
    Caption = 'Comunicazione tramite protocollo Host Link (RS232):'
    TabOrder = 0
    object BoxBaudrate: TRadioGroup
      Left = 102
      Top = 20
      Width = 136
      Height = 106
      Caption = 'Baudrate:'
      Columns = 2
      Items.Strings = (
        '1200'
        '2400'
        '4800'
        '9600'
        '19200'
        '38400'
        '57600'
        '115200')
      TabOrder = 0
    end
    object BoxCOM: TGroupBox
      Left = 12
      Top = 20
      Width = 79
      Height = 41
      Caption = 'Seriale:'
      TabOrder = 1
      object ListaCOM: TComboBox
        Left = 9
        Top = 15
        Width = 62
        Height = 21
        Style = csDropDownList
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 13
        ParentFont = False
        TabOrder = 0
        OnClick = ListaCOMClick
      end
    end
    object BoxDati: TRadioGroup
      Left = 367
      Top = 75
      Width = 54
      Height = 51
      Caption = 'Dati:'
      Items.Strings = (
        '7'
        '8')
      TabOrder = 2
    end
    object BoxParita: TRadioGroup
      Left = 247
      Top = 20
      Width = 111
      Height = 106
      Caption = 'Parit'#224':'
      Items.Strings = (
        'E - Pari'
        'O - Dispari'
        'N - Nessuna')
      TabOrder = 3
    end
    object BoxStopBit: TRadioGroup
      Left = 367
      Top = 20
      Width = 54
      Height = 51
      Caption = 'Stop bit:'
      Items.Strings = (
        '1'
        '2')
      TabOrder = 4
    end
    object BtnAttiva: TButton
      Left = 14
      Top = 100
      Width = 75
      Height = 25
      Caption = 'Attiva'
      TabOrder = 5
      OnClick = BtnAttivaClick
    end
    object BoxCtsRts: TCheckBox
      Left = 15
      Top = 70
      Width = 71
      Height = 17
      Caption = 'CTS/RTS'
      TabOrder = 6
    end
  end
end
