///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                    LibXL C++ headers version 4.6.0                        //
//                                                                           //
//                 Copyright (c) 2008 - 2025 XLware s.r.o.                   //
//                                                                           //
//   THIS FILE AND THE SOFTWARE CONTAINED HEREIN IS PROVIDED 'AS IS' AND     //
//                COMES WITH NO WARRANTIES OF ANY KIND.                      //
//                                                                           //
//          Please define LIBXL_STATIC variable for static linking.          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef LIBXL_ISHEETT_H
#define LIBXL_ISHEETT_H

#include "setup.h"
#include "enum.h"

namespace libxl
{

    template<class TCHAR> struct IFormatT;
    template<class TCHAR> struct IAutoFilterT;
    template<class TCHAR> struct IRichStringT;
    template<class TCHAR> struct IFormControlT;
    template<class TCHAR> struct IConditionalFormattingT;
    template<class TCHAR> struct ITableT;

    template<class TCHAR>
    struct ISheetT
    {
        virtual         CellType XLAPIENTRY cellType(int row, int col) const = 0;
        virtual             bool XLAPIENTRY isFormula(int row, int col) const = 0;

        virtual IFormatT<TCHAR>* XLAPIENTRY cellFormat(int row, int col) const = 0;
        virtual             void XLAPIENTRY setCellFormat(int row, int col, IFormatT<TCHAR>* format) = 0;

        virtual     const TCHAR* XLAPIENTRY readStr(int row, int col, IFormatT<TCHAR>** format = 0) = 0;
        virtual             bool XLAPIENTRY writeStr(int row, int col, const TCHAR* value, IFormatT<TCHAR>* format = 0, CellType type = CELLTYPE_STRING) = 0;

        virtual IRichStringT<TCHAR>* XLAPIENTRY readRichStr(int row, int col, IFormatT<TCHAR>** format = 0) = 0;
        virtual             bool XLAPIENTRY writeRichStr(int row, int col, IRichStringT<TCHAR>* richString, IFormatT<TCHAR>* format = 0) = 0;

        virtual           double XLAPIENTRY readNum(int row, int col, IFormatT<TCHAR>** format = 0) const = 0;
        virtual             bool XLAPIENTRY writeNum(int row, int col, double value, IFormatT<TCHAR>* format = 0) = 0;

        virtual             bool XLAPIENTRY readBool(int row, int col, IFormatT<TCHAR>** format = 0) const = 0;
        virtual             bool XLAPIENTRY writeBool(int row, int col, bool value, IFormatT<TCHAR>* format = 0, int errCode = ERRORTYPE_NOERROR) = 0;

        virtual             bool XLAPIENTRY readBlank(int row, int col, IFormatT<TCHAR>** format) const = 0;
        virtual             bool XLAPIENTRY writeBlank(int row, int col, IFormatT<TCHAR>* format) = 0;

        virtual     const TCHAR* XLAPIENTRY readFormula(int row, int col, IFormatT<TCHAR>** format = 0) = 0;
        virtual             bool XLAPIENTRY writeFormula(int row, int col, const TCHAR* expr, IFormatT<TCHAR>* format = 0) = 0;

        virtual             bool XLAPIENTRY writeFormulaNum(int row, int col, const TCHAR* expr, double value, IFormatT<TCHAR>* format = 0) = 0;
        virtual             bool XLAPIENTRY writeFormulaStr(int row, int col, const TCHAR* expr, const TCHAR* value, IFormatT<TCHAR>* format = 0) = 0;
        virtual             bool XLAPIENTRY writeFormulaBool(int row, int col, const TCHAR* expr, bool value, IFormatT<TCHAR>* format = 0) = 0;

        virtual     const TCHAR* XLAPIENTRY readComment(int row, int col) const = 0;
        virtual             void XLAPIENTRY writeComment(int row, int col, const TCHAR* value, const TCHAR* author = 0, int width = 129, int height = 75) = 0;
        virtual             void XLAPIENTRY removeComment(int row, int col) = 0;

        virtual             bool XLAPIENTRY isDate(int row, int col) const = 0;
        virtual             bool XLAPIENTRY isRichStr(int row, int col) const = 0;

        virtual        ErrorType XLAPIENTRY readError(int row, int col) const = 0;
        virtual             void XLAPIENTRY writeError(int row, int col, ErrorType error, IFormatT<TCHAR>* format = 0) = 0;

        virtual           double XLAPIENTRY colWidth(int col) const = 0;
        virtual           double XLAPIENTRY rowHeight(int row) const = 0;

        virtual              int XLAPIENTRY colWidthPx(int col) const = 0;
        virtual              int XLAPIENTRY rowHeightPx(int row) const = 0;

        virtual IFormatT<TCHAR>* XLAPIENTRY colFormat(int col) const = 0;
        virtual IFormatT<TCHAR>* XLAPIENTRY rowFormat(int row) const = 0;

        virtual             bool XLAPIENTRY setCol(int colFirst, int colLast, double width, IFormatT<TCHAR>* format = 0, bool hidden = false) = 0;
        virtual             bool XLAPIENTRY setColPx(int colFirst, int colLast, int widthPx, IFormatT<TCHAR>* format = 0, bool hidden = false) = 0;

        virtual             bool XLAPIENTRY setRow(int row, double height, IFormatT<TCHAR>* format = 0, bool hidden = false) = 0;
        virtual             bool XLAPIENTRY setRowPx(int row, int heightPx, IFormatT<TCHAR>* format = 0, bool hidden = false) = 0;

        virtual             bool XLAPIENTRY rowHidden(int row) const = 0;
        virtual             bool XLAPIENTRY setRowHidden(int row, bool hidden) = 0;

        virtual             bool XLAPIENTRY colHidden(int col) const = 0;
        virtual             bool XLAPIENTRY setColHidden(int col, bool hidden) = 0;

        virtual             double XLAPIENTRY defaultRowHeight() const = 0;
        virtual             void XLAPIENTRY setDefaultRowHeight(double height) = 0;


        virtual             bool XLAPIENTRY getMerge(int row, int col, int* rowFirst = 0, int* rowLast = 0, int* colFirst = 0, int* colLast = 0) = 0;
        virtual             bool XLAPIENTRY setMerge(int rowFirst, int rowLast, int colFirst, int colLast) = 0;
        virtual             bool XLAPIENTRY delMerge(int row, int col) = 0;

        virtual              int XLAPIENTRY mergeSize() const = 0;
        virtual             bool XLAPIENTRY merge(int index, int* rowFirst, int* rowLast, int* colFirst, int* colLast) = 0;
        virtual             bool XLAPIENTRY delMergeByIndex(int index) = 0;

        virtual              int XLAPIENTRY pictureSize() const = 0;
        virtual              int XLAPIENTRY getPicture(int index, int* rowTop = 0, int* colLeft = 0, int* rowBottom = 0, int* colRight = 0,
                                                                  int* width = 0, int* height = 0, int* offset_x = 0, int* offset_y = 0, const TCHAR** linkPath = 0) const = 0;
        virtual             bool XLAPIENTRY removePictureByIndex(int index) = 0;

        virtual             void XLAPIENTRY setPicture(int row, int col, int pictureId, double scale = 1.0, int offset_x = 0, int offset_y = 0, Position pos = POSITION_MOVE_AND_SIZE) = 0;
        virtual             void XLAPIENTRY setPicture2(int row, int col, int pictureId, int width = -1, int height = -1, int offset_x = 0, int offset_y = 0, Position pos = POSITION_MOVE_AND_SIZE) = 0;
        virtual             bool XLAPIENTRY removePicture(int row, int col) = 0;

        virtual              int XLAPIENTRY getHorPageBreak(int index) const = 0;
        virtual              int XLAPIENTRY getHorPageBreakSize() const = 0;

        virtual              int XLAPIENTRY getVerPageBreak(int index) const = 0;
        virtual              int XLAPIENTRY getVerPageBreakSize() const = 0;

        virtual             bool XLAPIENTRY setHorPageBreak(int row, bool pageBreak = true) = 0;
        virtual             bool XLAPIENTRY setVerPageBreak(int col, bool pageBreak = true) = 0;

        virtual             void XLAPIENTRY split(int row, int col) = 0;
        virtual             bool XLAPIENTRY splitInfo(int* row, int* col) const = 0;

        virtual             bool XLAPIENTRY groupRows(int rowFirst, int rowLast, bool collapsed = true) = 0;
        virtual             bool XLAPIENTRY groupCols(int colFirst, int colLast, bool collapsed = true) = 0;

        virtual             bool XLAPIENTRY groupSummaryBelow() const = 0;
        virtual             void XLAPIENTRY setGroupSummaryBelow(bool below) = 0;

        virtual             bool XLAPIENTRY groupSummaryRight() const = 0;
        virtual             void XLAPIENTRY setGroupSummaryRight(bool right) = 0;

        virtual             bool XLAPIENTRY clear(int rowFirst = 0, int rowLast = 1048575, int colFirst = 0, int colLast = 16383) = 0;

        virtual             bool XLAPIENTRY insertCol(int colFirst, int colLast, bool updateNamedRanges = true) = 0;
  virtual             bool XLAPIENTRY insertRow(int rowFirst, int rowLast, bool updateNamedRanges = true) = 0;
  virtual             bool XLAPIENTRY removeCol(int colFirst, int colLast, bool updateNamedRanges = true) = 0;
  virtual             bool XLAPIENTRY removeRow(int rowFirst, int rowLast, bool updateNamedRanges = true) = 0;

        virtual             bool XLAPIENTRY copyCell(int rowSrc, int colSrc, int rowDst, int colDst) = 0;

        virtual              int XLAPIENTRY firstRow() const = 0;
        virtual              int XLAPIENTRY lastRow() const = 0;
        virtual              int XLAPIENTRY firstCol() const = 0;
        virtual              int XLAPIENTRY lastCol() const = 0;

        virtual              int XLAPIENTRY firstFilledRow() const = 0;
        virtual              int XLAPIENTRY lastFilledRow() const = 0;
        virtual              int XLAPIENTRY firstFilledCol() const = 0;
        virtual              int XLAPIENTRY lastFilledCol() const = 0;

        virtual             bool XLAPIENTRY displayGridlines() const = 0;
        virtual             void XLAPIENTRY setDisplayGridlines(bool show = true) = 0;

        virtual             bool XLAPIENTRY printGridlines() const = 0;
        virtual             void XLAPIENTRY setPrintGridlines(bool print = true) = 0;

        virtual              int XLAPIENTRY zoom() const = 0;
        virtual             void XLAPIENTRY setZoom(int zoom) = 0;

        virtual              int XLAPIENTRY printZoom() const = 0;
        virtual             void XLAPIENTRY setPrintZoom(int zoom) = 0;

        virtual             bool XLAPIENTRY getPrintFit(int* wPages, int* hPages) const = 0;
        virtual             void XLAPIENTRY setPrintFit(int wPages = 1, int hPages = 1) = 0;

        virtual             bool XLAPIENTRY landscape() const = 0;
        virtual             void XLAPIENTRY setLandscape(bool landscape = true) = 0;

        virtual            Paper XLAPIENTRY paper() const = 0;
        virtual             void XLAPIENTRY setPaper(Paper paper = PAPER_DEFAULT) = 0;

        virtual     const TCHAR* XLAPIENTRY header() const = 0;
        virtual             bool XLAPIENTRY setHeader(const TCHAR* header, double margin = 0.5) = 0;
        virtual           double XLAPIENTRY headerMargin() const = 0;

        virtual     const TCHAR* XLAPIENTRY footer() const = 0;
        virtual             bool XLAPIENTRY setFooter(const TCHAR* footer, double margin = 0.5) = 0;
        virtual           double XLAPIENTRY footerMargin() const = 0;

        virtual             bool XLAPIENTRY hCenter() const = 0;
        virtual             void XLAPIENTRY setHCenter(bool hCenter = true) = 0;

        virtual             bool XLAPIENTRY vCenter() const = 0;
        virtual             void XLAPIENTRY setVCenter(bool vCenter = true) = 0;

        virtual           double XLAPIENTRY marginLeft() const = 0;
        virtual             void XLAPIENTRY setMarginLeft(double margin) = 0;

        virtual           double XLAPIENTRY marginRight() const = 0;
        virtual             void XLAPIENTRY setMarginRight(double margin) = 0;

        virtual           double XLAPIENTRY marginTop() const = 0;
        virtual             void XLAPIENTRY setMarginTop(double margin) = 0;

        virtual           double XLAPIENTRY marginBottom() const = 0;
        virtual             void XLAPIENTRY setMarginBottom(double margin) = 0;

        virtual             bool XLAPIENTRY printRowCol() const = 0;
        virtual             void XLAPIENTRY setPrintRowCol(bool print = true) = 0;

        virtual             bool XLAPIENTRY printRepeatRows(int* rowFirst, int* rowLast) = 0;
        virtual             void XLAPIENTRY setPrintRepeatRows(int rowFirst, int rowLast) = 0;

        virtual             bool XLAPIENTRY printRepeatCols(int* colFirst, int* colLast) = 0;
        virtual             void XLAPIENTRY setPrintRepeatCols(int colFirst, int colLast) = 0;

        virtual             bool XLAPIENTRY printArea(int* rowFirst, int* rowLast, int* colFirst, int* colLast) = 0;
        virtual             void XLAPIENTRY setPrintArea(int rowFirst, int rowLast, int colFirst, int colLast) = 0;

        virtual             void XLAPIENTRY clearPrintRepeats() = 0;
        virtual             void XLAPIENTRY clearPrintArea() = 0;

        virtual             bool XLAPIENTRY getNamedRange(const TCHAR* name, int* rowFirst, int* rowLast, int* colFirst, int* colLast, int scopeId = SCOPE_UNDEFINED, bool* hidden = 0) = 0;
        virtual             bool XLAPIENTRY setNamedRange(const TCHAR* name, int rowFirst, int rowLast, int colFirst, int colLast, int scopeId = SCOPE_UNDEFINED, bool hidden = false) = 0;
        virtual             bool XLAPIENTRY delNamedRange(const TCHAR* name, int scopeId = SCOPE_UNDEFINED) = 0;

        virtual              int XLAPIENTRY namedRangeSize() const = 0;
        virtual     const TCHAR* XLAPIENTRY namedRange(int index, int* rowFirst, int* rowLast, int* colFirst, int* colLast, int* scopeId = 0, bool* hidden = 0) = 0;

        virtual             bool XLAPIENTRY getTable(const TCHAR* name, int* rowFirst, int* rowLast, int* colFirst, int* colLast, int* headerRowCount, int* totalsRowCount) = 0;
        virtual              int XLAPIENTRY tableSize() const = 0;
        virtual     const TCHAR* XLAPIENTRY table(int index, int* rowFirst, int* rowLast, int* colFirst, int* colLast, int* headerRowCount, int* totalsRowCount) = 0;

        virtual  ITableT<TCHAR>* XLAPIENTRY addTable(const TCHAR* name, int rowFirst, int rowLast, int colFirst, int colLast, bool hasHeaders = false, TableStyle tableStyle = TABLESTYLE_MEDIUM2) = 0;
        virtual  ITableT<TCHAR>* XLAPIENTRY getTableByName(const TCHAR* name) = 0;
        virtual  ITableT<TCHAR>* XLAPIENTRY getTableByIndex(int index) = 0;

        virtual              int XLAPIENTRY hyperlinkSize() const = 0;
        virtual     const TCHAR* XLAPIENTRY hyperlink(int index, int* rowFirst = 0, int* rowLast = 0, int* colFirst = 0, int* colLast = 0) = 0;
        virtual             bool XLAPIENTRY delHyperlink(int index) = 0;
        virtual             void XLAPIENTRY addHyperlink(const TCHAR* hyperlink, int rowFirst, int rowLast, int colFirst, int colLast) = 0;
        virtual              int XLAPIENTRY hyperlinkIndex(int row, int col) const = 0;

        virtual             bool XLAPIENTRY isAutoFilter() const = 0;
        virtual IAutoFilterT<TCHAR>* XLAPIENTRY autoFilter() = 0;
        virtual             void XLAPIENTRY applyFilter() = 0;
        virtual             void XLAPIENTRY applyFilter2(IAutoFilterT<TCHAR>* autoFilter) = 0;
        virtual             void XLAPIENTRY removeFilter() = 0;

        virtual     const TCHAR* XLAPIENTRY name() const = 0;
        virtual             void XLAPIENTRY setName(const TCHAR* name) = 0;

        virtual             bool XLAPIENTRY protect() const = 0;
        virtual             void XLAPIENTRY setProtect(bool protect = true, const TCHAR* password = 0, EnhancedProtection prot = PROT_DEFAULT) = 0;

        virtual             SheetState XLAPIENTRY hidden() const = 0;
        virtual             bool XLAPIENTRY setHidden(SheetState state = SHEETSTATE_HIDDEN) = 0;

        virtual             void XLAPIENTRY getTopLeftView(int* row, int* col) const = 0;
        virtual             void XLAPIENTRY setTopLeftView(int row, int col) = 0;

        virtual             bool XLAPIENTRY rightToLeft() const = 0;
        virtual             void XLAPIENTRY setRightToLeft(bool rightToLeft = true) = 0;

        virtual             void XLAPIENTRY setAutoFitArea(int rowFirst = 0, int colFirst = 0, int rowLast = -1, int colLast = -1) = 0;

        virtual             void XLAPIENTRY addrToRowCol(const TCHAR* addr, int* row, int* col, bool* rowRelative = 0, bool* colRelative = 0) = 0;
        virtual     const TCHAR* XLAPIENTRY rowColToAddr(int row, int col, bool rowRelative = true, bool colRelative = true) = 0;

        virtual            Color XLAPIENTRY tabColor() const = 0;
        virtual             void XLAPIENTRY setTabColor(Color color) = 0;

        virtual             bool XLAPIENTRY getTabColor(int* red, int* green, int* blue) const = 0;
        virtual             void XLAPIENTRY setTabColor(int red, int green, int blue) = 0;

        virtual             bool XLAPIENTRY setBorder(int rowFirst, int rowLast, int colFirst, int colLast, BorderStyle borderStyle, Color borderColor) = 0;

        virtual             bool XLAPIENTRY addIgnoredError(int rowFirst, int colFirst, int rowLast, int colLast, IgnoredError iError) = 0;

        virtual             void XLAPIENTRY addDataValidation(DataValidationType type, DataValidationOperator op, int rowFirst, int rowLast, int colFirst, int colLast, const TCHAR* value1, const TCHAR* value2 = 0,
                                                              bool allowBlank = true, bool hideDropDown = false, bool showInputMessage = true, bool showErrorMessage = true, const TCHAR* promptTitle = 0, const TCHAR* prompt = 0,
                                                              const TCHAR* errorTitle = 0, const TCHAR* error = 0, DataValidationErrorStyle errorStyle = VALIDATION_ERRSTYLE_STOP) = 0;

        virtual             void XLAPIENTRY addDataValidationDouble(DataValidationType type, DataValidationOperator op, int rowFirst, int rowLast, int colFirst, int colLast, double value1, double value2,
                                                              bool allowBlank = true, bool hideDropDown = false, bool showInputMessage = true, bool showErrorMessage = true, const TCHAR* promptTitle = 0, const TCHAR* prompt = 0,
                                                              const TCHAR* errorTitle = 0, const TCHAR* error = 0, DataValidationErrorStyle errorStyle = VALIDATION_ERRSTYLE_STOP) = 0;

        virtual             void XLAPIENTRY removeDataValidations() = 0;

        virtual              int XLAPIENTRY formControlSize() const = 0;
        virtual IFormControlT<TCHAR>* XLAPIENTRY formControl(int index) = 0;

        virtual IConditionalFormattingT<TCHAR>* XLAPIENTRY addConditionalFormatting() = 0;

        virtual             bool XLAPIENTRY getActiveCell(int* row, int* col) const = 0;
        virtual             void XLAPIENTRY setActiveCell(int row, int col) = 0;

        virtual     const TCHAR* XLAPIENTRY selectionRange() const = 0;
        virtual             void XLAPIENTRY addSelectionRange(const TCHAR* sqref) = 0;
        virtual             void XLAPIENTRY removeSelection() = 0;

        virtual                             ~ISheetT() {}
    };

}

#endif
