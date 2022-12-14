// Geometric Tools
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

//----------------------------------------------------------------------------
template <class T>
void Allocate (int iCols, int iRows, T**& raatArray)
{
    raatArray = new T*[iRows];
    raatArray[0] = new T[iRows*iCols];
    for (int iRow = 1; iRow < iRows; iRow++)
    {
        raatArray[iRow] = &raatArray[0][iCols*iRow];
    }
}
//----------------------------------------------------------------------------
template <class T>
void Deallocate (T**& raatArray)
{
    delete[] raatArray[0];
    delete[] raatArray;
    raatArray = 0;
}
//----------------------------------------------------------------------------
template <class T>
void Allocate (int iCols, int iRows, int iSlices, T***& raaatArray)
{
    raaatArray = new T**[iSlices];
    raaatArray[0] = new T*[iSlices*iRows];
    raaatArray[0][0] = new T[iSlices*iRows*iCols];
    for (int iSlice = 0; iSlice < iSlices; iSlice++)
    {
        raaatArray[iSlice] = &raaatArray[0][iRows*iSlice];
        for (int iRow = 0; iRow < iRows; iRow++)
        {
            raaatArray[iSlice][iRow] =
                &raaatArray[0][0][iCols*(iRow+iRows*iSlice)];
        }
    }
}
//----------------------------------------------------------------------------
template <class T>
void Deallocate (T***& raaatArray)
{
    delete[] raaatArray[0][0];
    delete[] raaatArray[0];
    delete[] raaatArray;
    raaatArray = 0;
}
//----------------------------------------------------------------------------


