#ifndef _OA_BILL_MGR_H
#define _OA_BILL_MGR_H

#include "SdoBase.h"
#include "string_def.h"
#include "callback_queue.h"
#include "release_owner.h"
#include "id_object_pool.h"

struct MS_USER;
struct MS_ROLE_DATA;
struct MS_ONLINE_ROLE_DATA;

enum TimeBillingType
{
    etbtInvalid,
    etbtForceOffline,       //1: force logout
    etbtKeepOnline,         //2: remain online
    etbtPlayerGoOffline,    //3: voluntary logout
    etbtSupplementary,      //4: supplementary charge
    etbtTimeUp,             //5: account balance threshold
    etbtChangeScene,        //6: change map
    etbtTotal
};

struct SOARequest
{
    enum ERequestType
    {
        ertInvalid,
        ertQuery,
        ertCharge,
        ertFreeze,
        ertUnfreeze,
        ertConfirm,
        ertTransfer,
        ertQueryTime,
        ertSubTime,
        ertBuyTime,
        ertBuyTimeAuto
    };
    ERequestType eType;
    INT32 nGroupID;
    UINT32 dwRoleID;
    char szUserName[USER_NAME_LEN];
    char szUniqueID[OA_BILLING_UNIQUE_ID_LEN];
    UINT32 dwReqTime;
    UINT8 bOverTime;

    struct Charge
    {
        INT32 nItemID;
        INT32 nItemCount;
        INT32 nTotalCost;
        char szRoleName[ROLE_NAME_LEN];
    };
    struct Freeze
    {
        INT32 nPrice;
        INT32 nCount;
    };
    struct Unfreeze
    {
        INT32 nMoney;
    };
    struct Confirm
    {
        UINT64 qwOrderID;
        INT32 nCount;
        INT32 nMoney;
    };
    struct Transfer
    {
        UINT64 qwBuyingOrderID;
        UINT32 dwBuyerRoleID;
        INT32 nCount;
        INT32 nMoney;
    };
    struct SubTime
    {
        INT32 nTimeAmount;
        INT32 nStatus;
    };
    struct BuyTime
    {
        INT32 nGroupID;
        INT32 nPayAmount;
        INT32 nTimeAmount;
    };

    union
    {
        Charge charge;
        Freeze freeze;
        Unfreeze unfreeze;
        Confirm confirm;
        Transfer transfer;
        SubTime subtime;
        BuyTime buytime;
    };
};

#define BALANCE_STR_LEN 32
struct SOAResult
{
    UINT32 dwRpcId;
    INT32 nResult;
    char szBalanceInfo[BALANCE_STR_LEN];
};

class COABillMgr
{
public:
    static COABillMgr& GetInstance()
    {
        static COABillMgr ms_Instance;
        return ms_Instance;   
    }

    ~COABillMgr() { UnInit(); }
    BOOL Init();
    BOOL UnInit();
    BOOL MainLoop();

    BOOL DoQueryAccount(MS_USER* user);
    BOOL DoCharge(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nMoney, INT32 nItemID, INT32 nItemCount, const char* szRoleName);
    BOOL DoFreezeCurrency(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nMoney, INT32 nPrice, INT32 nCount);
    BOOL DoUnfreezeCurrency(UINT32 dwRoleID, const char* szBillingOrderID, INT32 nMoney, UINT64 qwOrderID);
    BOOL DoConfirmFrozenCurrency(UINT64 qwBuyingOrderID, UINT64 qwSellingOrderID, UINT32 dwRoleID, const char* szBillingOrderID, INT32 nCount, INT32 nMoney);
    BOOL DoTransferCurrency(UINT64 qwBuyingOrderID, UINT64 qwSellingOrderID, MS_ROLE_DATA* buyer, MS_ROLE_DATA* seller,
        const char* szBillingOrderID, INT32 nCount, INT32 nMoney, INT32 nTax);

    BOOL DoQueryTime(MS_USER* pstUser);
    BOOL DoSubTime(MS_ONLINE_ROLE_DATA* pstOnlineRole, UINT32 dwBeginTime, UINT32 dwEndTime, INT32 nTimeInterval, INT32 nStatus);
    BOOL DoBuyTime(MS_ONLINE_ROLE_DATA* pstOnlineRole, INT32 nPayAmount, INT32 nTimeAmount, BOOL bAuto);

    void OnSubTime(SOARequest* req, SOAResult* res);
    void OnBuyTime(SOARequest* req, SOAResult* res, BOOL bAuto);

    void OnBuyTime_Test(CHAR* szRoleName, SOAResult res, BOOL bAuto);

    void OnCallBack(SOAResult* res);

    ISDOCommonHandler* GetHandler() { return m_poOABill.get(); }

private:
    COABillMgr();

    CIDObjectPool<SOARequest, UINT32> m_RequestPool;
    cqyh::CallbackQueue<SOAResult> m_ResultList;
    cqyh::ReleasePtr<ISDOCommonHandler> m_poOABill;
};

#endif // _OA_BILL_MGR_H
