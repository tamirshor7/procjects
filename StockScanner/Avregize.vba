Option Explicit

Public Sub avregize()

    Dim first_unused_col_idx, stock_idx, dest_col As Integer
    Dim ws As Worksheet
    Dim match_col As Variant
    Dim init_required As Boolean
    init_required = False
    

    If sheetExists(gen_name) = False Then
        Worksheets.Add
        ActiveSheet.Name = gen_name
        ActiveSheet.Range("A1").Select
        init_required = True
        
    Else
        Worksheets(gen_name).Range("A1").Select
    End If
    
    
    
    first_unused_col_idx = usedCols(ActiveSheet) + 1
    
    For Each ws In Worksheets
        If Left(ws.Name, 5) = "Sheet" Or ws.Name = gen_name Then GoTo Cont 'ignore auto-generated sheets. Shouldn't exist - this is a precaution.
        If init_required = True Then
            Call sheetInit(ws, ActiveSheet)
            init_required = False
        End If
        
        match_col = Application.Match(ws.Name, Rows(1), 0) 'check to see if stock already exists in sheet. if so only update existing location.
        If VarType(match_col) <> DOUBLE_VARTYPE_INDICATOR Then 'use new available column
            dest_col = first_unused_col_idx
            first_unused_col_idx = first_unused_col_idx + 1
        Else
            dest_col = CInt(match_col)
        End If
        
        
        Call avregizeStock(ws, dest_col)
        
Cont:
    Next


End Sub

Function sheetExists(sheet_name As String) As Boolean
    Dim sheet_attempt As Worksheet
    On Error Resume Next
    Set sheet_attempt = Sheets(sheet_name)
    On Error GoTo 0
    sheetExists = Not sheet_attempt Is Nothing
End Function

Function sheetInit(ByRef source As Worksheet, ByRef dest As Worksheet)
        'Initialize dest's first column according to the stat header names from source
        
        Dim cur_idx As Integer
        
        dest.Cells(1, DEST_STATS_COL).Value = "Stats"
        dest.Cells(1, DEST_STATS_COL).Font.Bold = True
        
        
        For cur_idx = 2 To usedCols(source) 'ignore timestamp field
            dest.Cells(cur_idx, DEST_STATS_COL).Value = source.Cells(SOURCE_STATS_ROW, cur_idx).Value
        Next
    
End Function

Function usedRows(ByRef ws As Worksheet) As Integer
    'return number of used rows in given sheet
    usedRows = ws.Cells(Rows.Count, 1).End(xlUp).Row
End Function
Function usedCols(ByRef ws As Worksheet) As Integer
    'return number of used columns in given sheet
    usedCols = ws.Cells(1, Columns.Count).End(xlToLeft).Column
End Function

Function avregizeStock(ByRef stock As Worksheet, ByVal dest_col As Integer)
    Dim curr_stat_idx_source, curr_stat_idx_dest As Integer
    Dim curr_stat As String
    
    ActiveSheet.Cells(DEST_STOCK_ROW, dest_col).Value = stock.Name
    ActiveSheet.Cells(DEST_STOCK_ROW, dest_col).Font.Bold = True
    
    For curr_stat_idx_source = 2 To usedCols(stock)
        curr_stat = stock.Cells(SOURCE_STATS_ROW, curr_stat_idx_source).Value
        curr_stat_idx_dest = Application.Match(curr_stat, Columns(1), 0)
        ActiveSheet.Cells(curr_stat_idx_dest, dest_col).Value = calcAvg(stock, curr_stat_idx_source)
        
    Next
    
    
End Function

Function calcAvg(ByRef ws As Worksheet, ByVal col As Integer) As String
    'Calculate average on given column. Ignore null indicators in calculation
    
    Dim counter, curr_row As Integer
    Dim sum As Double
    Dim curr_value As Variant
    
    counter = 0
    sum = 0
    For curr_row = 2 To usedRows(ws)
    
        curr_value = ws.Cells(curr_row, col).Value
        If curr_value <> NULL_INDICATOR Then
            If IsNumeric(curr_value) Then
                sum = sum + CDbl(curr_value)
                counter = counter + 1
            Else
                curr_value = Left(curr_value, Len(curr_value) - 1) 'truncate last char to ignore numerical suffixes ('%', 'M', etc...)
                If IsNumeric(curr_value) Then 'better to duplicate these 3 lines than suffer function call overhead, given there's no inline mechanism like in C.
                    sum = sum + CInt(curr_value)
                    counter = counter + 1
                Else
                    GoTo break
                    
                End If
            End If
        End If
        
                
    Next
break:
    
    
    If counter = 0 Then
        calcAvg = NULL_INDICATOR
    Else
        calcAvg = CStr(sum / counter)
    End If
  

    
End Function
