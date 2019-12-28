VERSION 5.00
Begin VB.Form Form1 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Geomethod Vectorizer Automation Sample"
   ClientHeight    =   5304
   ClientLeft      =   36
   ClientTop       =   264
   ClientWidth     =   5328
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5304
   ScaleWidth      =   5328
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox OutputBox 
      CausesValidation=   0   'False
      Height          =   3120
      Left            =   120
      TabIndex        =   9
      Top             =   2040
      Width           =   5052
   End
   Begin VB.CommandButton SetDefault 
      Caption         =   "SetDefault"
      Height          =   372
      Left            =   4080
      TabIndex        =   8
      Top             =   360
      Width           =   1092
   End
   Begin VB.TextBox Options 
      Height          =   288
      Left            =   1560
      TabIndex        =   6
      Top             =   840
      Width           =   2292
   End
   Begin VB.TextBox OutputPath 
      Height          =   288
      Left            =   1560
      TabIndex        =   3
      Top             =   480
      Width           =   2292
   End
   Begin VB.TextBox InputPath 
      Height          =   288
      Left            =   1560
      TabIndex        =   2
      Top             =   120
      Width           =   2292
   End
   Begin VB.CommandButton Vectorize 
      Caption         =   "Vectorize"
      Height          =   372
      Left            =   2760
      TabIndex        =   1
      Top             =   1440
      Width           =   1092
   End
   Begin VB.CommandButton GetVersion 
      Caption         =   "GetVersion"
      Height          =   372
      Left            =   120
      TabIndex        =   0
      Top             =   1440
      Width           =   1092
   End
   Begin VB.Label Label3 
      Caption         =   "Options:"
      Height          =   252
      Left            =   120
      TabIndex        =   7
      Top             =   840
      Width           =   1212
   End
   Begin VB.Label Label2 
      Caption         =   "Output File:"
      Height          =   252
      Left            =   120
      TabIndex        =   5
      Top             =   480
      Width           =   1212
   End
   Begin VB.Label Label1 
      Caption         =   "Input File:"
      Height          =   252
      Left            =   120
      TabIndex        =   4
      Top             =   120
      Width           =   1212
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim vectorizerDoc As GMVectorizer.Document

Private Sub Form_Load()
Set vectorizerDoc = CreateObject("GMVectorizer.Document")
Call SetDefault_Click
End Sub

Private Sub GetVersion_Click()
MsgBox (vectorizerDoc.GetVersion)
End Sub

Private Sub Vectorize_Click()
Form1.MousePointer = vbHourglass
OutputBox.Clear
vectorizerDoc.Open (InputPath.Text)
'vectorizerDoc.Options = Options.Text
If vectorizerDoc.Vectorize Then
s = "Vectorization succeed"
Else
s = "Vectorization failed"
End If
OutputBox.AddItem (s)
vectorizerDoc.Save (OutputPath.Text)
Form1.MousePointer = vbDefault
End Sub

Private Sub SetDefault_Click()
InputPath.Text = App.Path + "\BWSample.gif"
OutputPath.Text = App.Path + "\BWSample.dxf"
Options.Text = ""
End Sub
