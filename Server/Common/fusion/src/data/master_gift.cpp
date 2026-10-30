#include "stdafx.h"
#include "read_data_helper.h"
#include "master_gift.h"

BOOL MS_MASTER_GIFT_TEMPLATE::ReadLine(INT32& rnID, const char*& rpcszName, MS_MASTER_GIFT_TEMPLATE* pData, CTabFile* pTabConfig, INT32 nRow)
{
    READ_START();

    READ_INT("ID", m_dwLevel);
    READ_INT("dwLvlUpStudMailID", m_dwLvlUpStudMailID);
    READ_INT("dwLvlUpTeacherMailID", m_dwLvlUpTeacherMailID);
    READ_INT("arrCZMoneyConsume0", m_arrCZMoneyConsume[0]);
    READ_INT("arrCZMoneyConsume1", m_arrCZMoneyConsume[1]);
    READ_INT("arrCZMoneyConsume2", m_arrCZMoneyConsume[2]);

    READ_INT("arrCZT2STeacherMailID0", m_arrCZT2STeacherMailID[0]);
    READ_INT("arrCZT2STeacherMailID1", m_arrCZT2STeacherMailID[1]);
    READ_INT("arrCZT2STeacherMailID2", m_arrCZT2STeacherMailID[2]);
    READ_INT("arrCZT2SStudentMailID0", m_arrCZT2SStudentMailID[0]);
    READ_INT("arrCZT2SStudentMailID1", m_arrCZT2SStudentMailID[1]);
    READ_INT("arrCZT2SStudentMailID2", m_arrCZT2SStudentMailID[2]);

    READ_INT("arrCZS2TTeacherMailID0", m_arrCZS2TTeacherMailID[0]);
    READ_INT("arrCZS2TTeacherMailID1", m_arrCZS2TTeacherMailID[1]);
    READ_INT("arrCZS2TTeacherMailID2", m_arrCZS2TTeacherMailID[2]);
    READ_INT("arrCZS2TStudentMailID0", m_arrCZS2TStudentMailID[0]);
    READ_INT("arrCZS2TStudentMailID1", m_arrCZS2TStudentMailID[1]);
    READ_INT("arrCZS2TStudentMailID2", m_arrCZS2TStudentMailID[2]);

    READ_INT("arrCZC2CSelfMailID0", m_arrCZC2CSelfMailID[0]);
    READ_INT("arrCZC2CSelfMailID1", m_arrCZC2CSelfMailID[1]);
    READ_INT("arrCZC2CSelfMailID2", m_arrCZC2CSelfMailID[2]);
    READ_INT("arrCZC2CTargetMailID0", m_arrCZC2CTargetMailID[0]);
    READ_INT("arrCZC2CTargetMailID1", m_arrCZC2CTargetMailID[1]);
    READ_INT("arrCZC2CTargetMailID2", m_arrCZC2CTargetMailID[2]);

    READ_END(m_dwLevel);
}



const MS_MASTER_GIFT_TEMPLATE* GetMasterGiftTemplate(INT8 nTemplateID)
{
    return CDataManager<MS_MASTER_GIFT_TEMPLATE>::GetInstance().GetData(nTemplateID);
}











