#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
	include
****************************************************************/
#include "base_mw_modechange.h"
#include "base_mw_region.h"
#include "base_mw_overlay.h"
#include "base_venc_handle.h"
#include "base_mw_boardinterface.h"

/****************************************************************
	macro / enum
****************************************************************/

//#define USE_MODECHANGE_MW_LOG
#ifdef USE_MODECHANGE_MW_LOG
#define debug_log(fmt, args...) fprintf(stdout, "[ Mode Change MW ] : "fmt, ##args)
#define debug_log_funcIn() fprintf(stdout, "in %s\n", __func__)
#define debug_log_funcOut() fprintf(stdout, "out %s\n", __func__)
#define debug_log_printPara(para) fprintf(stdout, "%s : 0x%x(%d) @ %p\n", #para, (para), (para), &(para))
#define debug_log_showErr(err) fprintf(stderr, "%s <%d> : Error <%d>\n", __func__, __LINE__, err)
#else
#define debug_log(fmt, args...)
#define debug_log_funcIn()
#define debug_log_funcOut()
#define debug_log_printPara(para)
#define debug_log_showErr(err)
#endif

/*get vi chn obey venc chn(high mode)*/
#define BASE_HIGH_MODE_GETVICH(vencCh) ((!vencCh)? 0:2)

#define BASE_VO_NULL 0
/****************************************************************
	variable
****************************************************************/

/****************************************************************
	prototype
****************************************************************/

/****************************************************************
	function
****************************************************************/
/*******************************************************************************
* @brief		: LONG lBASE_MODE_INIT(eBASE_MODEFLAG eMode)
* @param		: eBASE_MODEFLAG eMode
* @retval		: LONG
* @note 		: init mode
*******************************************************************************/
LONG lBASE_MODE_INIT(eBASE_MODEFLAG eMode){
	LONG lRet=BASE_RET_SUCCESS;

	debug_log_funcIn();

	/*****************************************/
	{	

		/* init global */
		vBASE_CH_INIT();

		/* init according to the input mode */
		switch(eMode){
			case BASE_MODE_VENC1080_VONULL_L:
				vBASE_VI_INIT_CH(&gsVi[BASE_VI_CH0], BASE_VI_CH0, -1, 30, BASE_RES_HD1080, BASE_VICH_BASE);	
				lBASE_VPSS_INIT_CH(&gsVpss[BASE_VPSS_CH0], BASE_VI_CH0, BASE_VPSS_CH0, BASE_VPSS_TYPE_BASE, BASE_RES_HD1080);
				vBASE_VENC_INIT_CH(&gsVenc[BASE_VENC_CH0], BASE_VENC_CH0);
				eBASE_MW_REGION_INIT(&gsRegCov[BASE_VENC_CH0], BASE_VENC_CH0, BASE_VI_CH0);
				break;

			case BASE_MODE_VENC720_VONULL_L:
				vBASE_VI_INIT_CH(&gsVi[BASE_VI_CH0], BASE_VI_CH0, -1, 30, BASE_RES_HD1080, BASE_VICH_BASE);	
				lBASE_VPSS_INIT_CH(&gsVpss[BASE_VPSS_CH1], BASE_VI_CH0, BASE_VPSS_CH1, BASE_VPSS_TYPE_BASE, BASE_RES_HD720);
				vBASE_VENC_INIT_CH(&gsVenc[BASE_VENC_CH1], BASE_VENC_CH1);
				eBASE_MW_REGION_INIT(&gsRegCov[BASE_VENC_CH1], BASE_VENC_CH0, BASE_VI_CH1);
				break;

			default:
				break;
		}

	}
	/*****************************************/

	debug_log_funcOut();
	
	return lRet;
}

/*******************************************************************************
* @brief		: LONG lBASE_MODE_START()
* @param		: 
* @retval		: LONG
* @note 		: start mode
*******************************************************************************/

LONG lBASE_MODE_START(){
	LONG lRet=BASE_RET_SUCCESS;
	LONG lCh=0;
	
	debug_log_funcIn();

	/*****************************************/
	{	
	
		/* start vpss channel */
		for(lCh=0;lCh!=BASE_VPSS_CHALL;lCh++){
			if(gsVpss[lCh].eState!=BASE_NOTUSE){
				lRet=lBASE_VPSS_START_CH(&gsVpss[lCh]);
				if(lRet!=BASE_RET_SUCCESS){
					return lRet;
				}
			}
		}

		/* start venc channel */
		for(lCh=0;lCh!=BASE_VENC_CHALL;lCh++){
			if(gsVenc[lCh].eState!=BASE_NOTUSE){
				lRet=lBASE_VENC_START_CH(&gsVenc[lCh]);
				if(lRet!=BASE_RET_SUCCESS){
					return lRet;
				}
			}
		}

	}
	/*****************************************/

	debug_log_funcOut();
	
	return lRet;
}
/*******************************************************************************
* @brief		: LONG lBASE_MODE_CHANGE(eBASE_MODEFLAG* peMode, eBASE_CH_CHANGE eChange)
* @param		: eBASE_MODEFLAG* peMode, eBASE_CH_CHANGE eChange
* @retval		: LONG
* @note 		: change mode
*******************************************************************************/

LONG lBASE_MODE_CHANGE(eBASE_MODEFLAG* peMode, eBASE_CH_CHANGE eChange){
	LONG lRet = BASE_RET_SUCCESS;
	
	debug_log_funcIn();

	/*****************************************/
	{
		/* this version is for carcarry project, only support following cases */
		if(*peMode != BASE_MODE_VENC1080_VONULL_L
			|| eChange != BASE_CH_VENC1080P) {
			return BASE_RET_FAIL;
		}
	}
	/*****************************************/

	debug_log_funcOut();
	
	return lRet;
}

/*******************************************************************************
* @brief		: eBASE_RET_STATE eBASE_VI_GETVICH(eBASE_MODEFLAG* peMode, LONG lVencCh, LONG* plViCh)
* @param		: eBASE_MODEFLAG* peMode, LONG lVencCh, LONG* plViCh
* @retval		: eBASE_RET_STATE
* @note 		: get vi chn obey mode and venc chn
*******************************************************************************/
eBASE_RET_STATE eBASE_VI_GETVICH(eBASE_MODEFLAG* peMode, LONG lVencCh, LONG* plViCh){
	eBASE_RET_STATE eRet=BASE_RET_SUCCESS;
	LONG lVencFlag=0;
	LONG lModeFlag=0;

	/* init to invalid channel */
	*plViCh=-1;
	
	debug_log_funcIn();

	/*****************************************/
	{
	
		/* get current vi, venc, mode flag */
		lVencFlag=BASE_COMM_GETTYPEVAL(*peMode, BASE_MODEFLAG_VENC);
		lModeFlag=BASE_COMM_GETTYPEVAL(*peMode, BASE_MODEFLAG_MODE);

		if(!(COMM_BIT(lVencCh)&lVencFlag)){
			/* indicate the venc ch not start yet, error */
			return BASE_RET_FAIL;
		}

		if(!lModeFlag){
			/* indicate low mode */
			*plViCh=lVencCh;
		} else {
			/* indicate high mode */
			*plViCh=(lVencCh==0)?0:2;
		}
		
	}
	/*****************************************/

	debug_log_funcOut();
	
	return eRet;
}

#ifdef __cplusplus
}
#endif
