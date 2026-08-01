// Filename: systemmanager.c
//
// Project: Horizon

#include "common.h"
#include "error.h"
#include "state.h"
#include "systemmanager.h"
#include "fbmanager.h"
#include "vbmanager.h"
#include "shadermanager.h"
#include "texmanager.h"
#include "georegmap.h"
#include "statevalidator.h"
#include "garent.h"
#include "commandbuffer.h"
#include "texform.h"
#include "picaregmap.h"
#include "bitmask.h"

sys_manager_t		__sysman;
dmpgl_allocator_t	__dmpgl_allocator;
dmpgl_deallocator_t	__dmpgl_deallocator;
static GLuint __miniCLID;
static GLuint __miniDBID;
static void* __manager_mem_block;
static GLsizei __initialCommandSize;

#define __GET_CL_LIST(cmdlist, cllist)	\
{	\
	for (cllist = __sysman.cl_list_table[cmdlist & CL_LIST_TABLE_SIZE]; cllist != 0; cllist = cllist->next)	\
	{	\
		if (cllist->id == cmdlist)	\
			break;	\
	}	\
}

#define __GET_DB_LIST(buffer, dblist)	\
{	\
	for (dblist = __sysman.db_list_table[buffer & DB_LIST_TABLE_SIZE]; dblist != 0; dblist = dblist->next)	\
	{	\
		if (dblist->id == buffer)	\
			break;	\
	}	\
}

static void sys_executeCommandRequest(cl_cmdreq_t* cmdreq);
static void sys_P3DCallback();
static void sys_PSC0Callback();
static void sys_PSC1Callback();
static void sys_PPFCallback();
static void sys_DMACallback();
static void sys_PDC0Callback();
static void sys_PDC1Callback();
static void sys_commonIntrHandler();

static void sys_issueSpeculativeCommandRequests();

static void sys_fixSubroutineCommand(cl_list_t* cmdlist, cl_cmdreq_t* cmdreq);
static void sys_fixSubroutineCommandNoCacheFlush(cl_list_t* cmdlist, cl_cmdreq_t* cmdreq);

enum SUBROUTINE_COMMAND_DATA_INDEX{
	SUBROUTINE_COMMAND_CH1_SIZE,
	SUBROUTINE_COMMAND_HEADER,
	SUBROUTINE_COMMAND_CH2_SIZE,
	SUBROUTINE_COMMAND_CH1_ADDR,
	SUBROUTINE_COMMAND_CH2_ADDR,
	SUBROUTINE_COMMAND_CH1_KICK,
	SUBROUTINE_COMMAND_NUM
};

inline void sys_CmdlistStorageCore(cl_list_t* cmdlist, GLsizei bufsize, GLvoid* commandbuffer, GLsizei requestcount, GLvoid* commandrequest);
inline void sys_RunCmdlistCore(cl_list_t* cmdlist);
inline void sys_ReserveStopCmdlistCore(cl_list_t* cmdlist, GLint id);
inline void sys_ClearCmdlistCore(cl_list_t* cmdlist);
inline void sys_SetCmdlistCallbackCore(cl_list_t* cmdlist, void (*func)(GLint));
inline void sys_EnableCmdlistCallbackCore(cl_list_t* cmdlist, GLint id);
inline void sys_DisableCmdlistCallbackCore(cl_list_t* cmdlist, GLint id);
inline void sys_FilterBlockImageCore(cl_list_t* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei width, GLsizei height, GLenum format);
inline void sys_CopyCmdlistCore(cl_list_t* scmdlist, cl_list_t* dcmdlist);
inline void sys_AddVramDmaCommandCore(cl_list_t* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei size);
inline void sys_AddVramDmaCommandNoCacheFlushCore(cl_list_t* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei size);
inline void sys_ClearFillCmdlistCore(cl_list_t* cmdlist, GLuint data);
inline void sys_SetGasAutoAccumulationUpdateCore(cl_list_t* cmdlist, GLint id);
inline void sys_ExportCmdlistCore(cl_list_t* cmdlist, GLuint bufferoffset, GLsizei buffersize, GLuint requestid, GLsizei requestsize, GLvoid* data);
inline void sys_AddL2BTransferCommandCore(cl_list_t* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei width, GLsizei height, GLenum format, GLsizei blocksize);
inline void sys_AddBlockImageCopyCommandCore(cl_list_t* cmdlist, const GLvoid* srcaddr, GLsizei srcunit, GLsizei srcinterval, GLvoid* dstaddr, GLsizei dstunit, GLsizei dstinterval, GLsizei totalsize);
inline void sys_AddMemoryFillCommandCore(cl_list_t* cmdlist, GLvoid* startaddr0, GLsizei size0, GLuint data0, GLsizei width0, GLvoid* startaddr1, GLsizei size1, GLuint data1, GLsizei width1);
inline void sys_AddB2LTransferCommandCore(cl_list_t* cmdlist, const GLvoid* srcaddr, GLsizei srcwidth, GLsizei srcheight, GLenum srcformat,GLvoid* dstaddr, GLsizei dstwidth, GLsizei dstheight, GLenum dstformat, GLenum aamode, GLboolean yflip, GLsizei blocksize);

GLboolean nngxInitialize(GLvoid* (*allocator)(GLenum, GLenum, GLuint, GLsizei), void (*deallocator)(GLenum, GLenum, GLuint, GLvoid*)){
	GLuint cmdlist = 1; /* set cmdlist to initialize HW */
	
	if (!allocator || !deallocator || __sysman.initialized)
		return GL_FALSE;
	
	memset(&__sysman, 0, sizeof(sys_manager_t));
	__sysman.display_width[0] = DISP0_WIDTH;
	__sysman.display_height[0] = DISP0_HEIGHT;
	__sysman.display_width[1] = DISP1_WIDTH;
	__sysman.display_height[1] = DISP1_HEIGHT;
	__dmpgl_allocator = allocator;
	__dmpgl_deallocator = deallocator;
	__miniCLID = __miniDBID = 1;
	__sysman.extension_mode = NN_GX_DISPLAYMODE_NORMAL;

	for (;;){
		GLboolean isfirst;
		stt_state_t* glstate;
		fb_manager_t* fbman;
		vb_manager_t* vbman;
		shader_manager_t* shman;
		tex_manager_t* texman;
		vld_validator_frag_info_t* finfo;
		
		{
			int size, offset;
			
			size = 0;
			size += sizeof(stt_state_t);
			size += sizeof(fb_manager_t);
			size += sizeof(vb_manager_t);
			size += sizeof(shader_manager_t);
			size += sizeof(tex_manager_t);
			size += sizeof(vld_validator_frag_info_t);
			
			__manager_mem_block = malloc(size);
			if (!__manager_mem_block)
				break;
			
			offset = 0;
			glstate = (stt_state_t*)((unsigned char*)__manager_mem_block + offset);
			offset += sizeof(stt_state_t);
			fbman = (fb_manager_t*)((unsigned char*)__manager_mem_block + offset);
			offset += sizeof(fb_manager_t);
			vbman = (vb_manager_t*)((unsigned char*)__manager_mem_block + offset);
			offset += sizeof(vb_manager_t);
			shman = (shader_manager_t*)((unsigned char*)__manager_mem_block + offset);
			offset += sizeof(shader_manager_t);
			texman = (tex_manager_t*)((unsigned char*)__manager_mem_block + offset);
			offset += sizeof(tex_manager_t);
			finfo = (vld_validator_frag_info_t*)((unsigned char*)__manager_mem_block + offset);
			offset += sizeof(vld_validator_frag_info_t);
		}
		
		if (__sta_initializeState(glstate) < 0)
			break;
		if (__fb_initializeFBManager(fbman) < 0)
			break;
		if (__vb_initializeVBManager(vbman) < 0)
			break;
		if (__shm_initializeShaderManager(shman) < 0)
			break;
		if (__tx_initializeTexManager(texman) < 0)
			break;
		
		glGetError();
		nngxBindCmdlist(cmdlist);
		nngxCmdlistStorage(0x10000, 16);
		nngxSetCmdlistParameteri(NN_GX_CMDLIST_RUN_MODE, NN_GX_CMDLIST_SERIAL_RUN);
		if (glGetError())
			break;
		if (__vldtr_initializeStateValidator(finfo) < 0)
			break;

		nngxGetCmdlistParameteri(NN_GX_CMDLIST_USED_BUFSIZE, (GLint*)&__initialCommandSize);
		/* succeeded initializing */
		
		/* initialize PICA */
		{
			unsigned garnet_write_data, garnet_write_mask;
			
			#define _GARNET_REG_WRITE(addr, value, be) \
			do \
			{ \
				garnet_write_data = value; \
				if ((be) == 0xf) \
					nngxlowWriteHWRegs((addr), (const void*)&garnet_write_data, 4); \
				else \
				{ \
					garnet_write_mask = 0; \
					if ((be) & 1) garnet_write_mask |= 0x000000ff; \
					if ((be) & 2) garnet_write_mask |= 0x0000ff00; \
					if ((be) & 4) garnet_write_mask |= 0x00ff0000; \
					if ((be) & 8) garnet_write_mask |= 0xff000000; \
					nngxlowWriteHWRegsWithMask((addr), (const void*)&garnet_write_data, (const void*)&garnet_write_mask, 4); \
				} \
			} while (0)

			nngxlowInitialize();
			isfirst = nngxlowIsFirstInitialization();
			__sysman.num_speculative = nngxlowGetNumSpeculativeRequests();
			
			/* register interrupt handlers */
			nngxlowRegisterInterruptHandler(sys_PSC0Callback, NN_GXLOW_INTERRUPT_GPU_PSC_0);
			nngxlowRegisterInterruptHandler(sys_PSC1Callback, NN_GXLOW_INTERRUPT_GPU_PSC_1);
			nngxlowRegisterInterruptHandler(sys_PDC0Callback, NN_GXLOW_INTERRUPT_GPU_PDC_0);
			nngxlowRegisterInterruptHandler(sys_PDC1Callback, NN_GXLOW_INTERRUPT_GPU_PDC_1);
			nngxlowRegisterInterruptHandler(sys_PPFCallback,  NN_GXLOW_INTERRUPT_GPU_PPF);
			nngxlowRegisterInterruptHandler(sys_P3DCallback,  NN_GXLOW_INTERRUPT_GPU_P3D);
			nngxlowRegisterInterruptHandler(sys_DMACallback,  NN_GXLOW_INTERRUPT_DMAC_0);

			if (isfirst){
				_GARNET_REG_WRITE(GARNET_ADDR_P3D(PA_RW_SYS_BACK_REG0), 0, 0xf);
				_GARNET_REG_WRITE(GARNET_ADDR_P3D(PA_RW_SYS_INT_VAL0), PICA_3D_INTR_CODE, 0xf);
				_GARNET_REG_WRITE(GARNET_ADDR_P3D(PA_RW_SYS_INT_MASK0), 0xfffffff0, 0xf);
				_GARNET_REG_WRITE(GARNET_ADDR_P3D(PA_RW_SYS_INT_CLK), 1, 0xf);	/* Low Power Consumption mode on */

				{
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_COUNT_SIZE_0),                         450, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_ADDR_TIME_START_0),                    209, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_RIGHT_BORDER_START_0),                 449, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_BLANK_START_0),                        449, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_SYNC_START_0),                           0, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_BACK_PORCH_START_0),                   207, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_LEFT_BORDER_START_0),                  209, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_INTERRUPT_TIMING_0),     (453 << 16) | 449, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_DMA_TIMING_0),           (  1 << 16) |   0, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_COUNTER_SIZE_0),                       413, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_ADDR_TIME_START_0),                      2, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_BOTTOM_BORDER_START_0),                402, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_BLANK_START_0),                        402, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_SYNC_START_0),                         402, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_BACK_PORCH_START_0),                     1, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_TOP_BORDER_START_0),                     2, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_INTERRUPT_TIMING_0),     (406 << 16) | 402, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_INCREMENT_H_VALUE_0),                    0, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_SIGNAL_POL_0),                             0, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_OUT_SIZE_0),               (400 << 16) | 240, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_PIC_BORDER_H_0),           (449 << 16) | 209, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_PIC_BORDER_V_0),           (402 << 16) |   2, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_MODE_0),                             0x80340, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_REG_UPDATE_0),             (  0 << 16) |   0, 0xf);

					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_COUNT_SIZE_1),                         450, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_ADDR_TIME_START_1),                    209, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_RIGHT_BORDER_START_1),                 449, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_BLANK_START_1),                        449, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_SYNC_START_1),                         205, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_BACK_PORCH_START_1),                   207, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_LEFT_BORDER_START_1),                  209, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_INTERRUPT_TIMING_1),     (453 << 16) | 449, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_H_DMA_TIMING_1),           (  1 << 16) |   0, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_COUNTER_SIZE_1),                       413, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_ADDR_TIME_START_1),                     82, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_BOTTOM_BORDER_START_1),                402, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_BLANK_START_1),                        402, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_SYNC_START_1),                          79, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_BACK_PORCH_START_1),                    80, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_TOP_BORDER_START_1),                    82, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_INTERRUPT_TIMING_1),     (408 << 16) | 404, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_V_INCREMENT_H_VALUE_1),                    0, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_SIGNAL_POL_1),                  (1 << 4) | 1, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_OUT_SIZE_1),               (320 << 16) | 240, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_PIC_BORDER_H_1),           (449 << 16) | 209, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_PIC_BORDER_V_1),           (402 << 16) |  82, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_MODE_1),                             0x80300, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_REG_UPDATE_1),             (  0 << 16) |   0, 0xf);

					/* set temporal address for frame buffer */
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_FB0_ADR_0), 0x18300000, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_FB1_ADR_0), 0x18300000, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_FB0_ADR_1), 0x18300000, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_FB1_ADR_1), 0x18300000, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_FB0_B_ADR_0), 0x18300000, 0xf);
					_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_FB1_B_ADR_0), 0x18300000, 0xf);
				}
				_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_SWAP_0), 1, 0xf);
				_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_SWAP_1), 1, 0xf);
				
				/* PPF interrupt clear */
				_GARNET_REG_WRITE(GARNET_ADDR_PPF(GARNET_REG_PPF_START), 0, 0x2);

				_GARNET_REG_WRITE(GARNET_ADDR_PSC(GARNET_REG_PSC_CLK_MODE), 0x00070100, 0xf);
				_GARNET_REG_WRITE(GARNET_ADDR_PSC(GARNET_REG_PSC_FILL_CTRL_0), 0, 0x1);
				_GARNET_REG_WRITE(GARNET_ADDR_PSC(GARNET_REG_PSC_FILL_CTRL_1), 0, 0x1);
				_GARNET_REG_WRITE(GARNET_ADDR_PSC(GARNET_REG_PSC_PMI_PRIORITY_4P), 0x22221200, 0xf);
				_GARNET_REG_WRITE(GARNET_ADDR_PSC(GARNET_REG_PSC_PMI_PRIORITY_FCRAM_S01_3P), 0x00000ff2, 0x3);

				_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_START_0), 0x00010501, 0xf);
				_GARNET_REG_WRITE(GARNET_ADDR_PDC(GARNET_REG_PDC_START_1), 0x00010501, 0xf);
			}
		}
		
		nngxSplitDrawCmdlist();
		{
			cl_list_t* cllist = __sysman.bound_cmdlist;
			cl_cmdreq_t* cmdreq;
			unsigned dstaddr;
			
			if (isfirst){
				cmdreq = &cllist->command_request[cllist->used_reqcount];
				cmdreq->id = CL_CMDREQ_ID_PF;
				cmdreq->param.pf.sformat = GARNET_PPF_FORMAT_R4G4B4A4;
				cmdreq->param.pf.srcaddr = (unsigned)nngxGetVramEndAddr(NN_GX_MEM_VRAMB) + 1 - 128 * 128 * 2;
				
				dstaddr = (unsigned)((unsigned char*)malloc_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_SYSTEM, 0, 128 * 128 * 2 + 16));
				if (dstaddr == 0)
					break;
				cmdreq->param.pf.dstaddr = dstaddr;
				if (cmdreq->param.pf.dstaddr & 0xf)
					cmdreq->param.pf.dstaddr += 0x10 - (cmdreq->param.pf.dstaddr & 0xf);
				cmdreq->param.pf.dformat = GARNET_PPF_FORMAT_R4G4B4A4;
				cmdreq->param.pf.swidth = 128;
				cmdreq->param.pf.sheight = 128;
				cmdreq->param.pf.dwidth = 128;
				cmdreq->param.pf.dheight = 128;
				cmdreq->param.pf.yflip = 0;
				cmdreq->param.pf.amode = GARNET_PPF_AMODE_NOP;
				cmdreq->param.pf.blk32 = 0;
				cmdreq->param.pf.l2b = 0;
				cmdreq->param.pf.b2b = 0;
				cllist->used_reqcount++;
			}
			
			nngxRunCmdlist();
			nngxWaitCmdlistDone();
			if (isfirst)
				free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_SYSTEM, 0, (void*)dstaddr);
		}
		nngxStopCmdlist();
		nngxBindCmdlist(0);
		#ifndef _GEN_TEST_VECTOR
		/* if make testvector, it's necessary to keep the initialization command. */
		nngxDeleteCmdlists(1, &cmdlist);
		#endif
		
		__sysman.initialized = GL_TRUE;
		
		return GL_TRUE;
	}
	
	nngxDeleteCmdlists(1, &cmdlist);

	return GL_FALSE;
}

void nngxFinalize(void){
	if (__sysman.initialized)
	{
		int i;

		__vldtr_finalizeStateValidator();
		__tx_finalizeTexManager();
		__shm_finalizeShaderManager();
		__vb_finalizeVBManager();
		__fb_finalizeFBManager();
		__sta_finalizeState();
		__sysman.initialized = GL_FALSE;

		nngxlowFinalize();

		for (i = 0; i < CL_LIST_TABLE_SIZE + 1; i++){ // do CL list first then DL list
			cl_list_t* cllist;
			cl_list_t* next;
			for (cllist = __sysman.cl_list_table[i]; cllist != 0; cllist = next){
				next = cllist->next;
				if (cllist->command_buffer)
					free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_COMMANDBUFFER, cllist->id, cllist->command_buffer);
				free(cllist);
			}
		}

		for (i = 0; i < DB_LIST_TABLE_SIZE + 1; i++){
			db_list_t* dblist;
			db_list_t* next;
			for (dblist = __sysman.db_list_table[i]; dblist != 0; dblist = next){
				next = dblist->next;
				if (dblist->address)
					free_ext(dblist->area, NN_GX_MEM_DISPLAYBUFFER, dblist->id, dblist->address);
				free(dblist);
			}
		}
		free(__manager_mem_block);
		__manager_mem_block = 0;
		__dmpgl_allocator = 0;
		__dmpgl_deallocator = 0;
	}
	
	return;
}

void nngxGetAllocator(GLvoid* (**allocator)(GLenum, GLenum, GLuint, GLsizei), void (**deallocator)(GLenum, GLenum, GLuint, GLvoid*)){
	if (allocator)
		*allocator = __dmpgl_allocator;
	if (deallocator)
		*deallocator = __dmpgl_deallocator;
	return;
}

void nngxGenCmdlists(GLsizei n, GLuint* cmdlists){
	GLint i;
	cl_list_t* cllist;
	cl_list_t* tmp;
	cl_list_t* newlist;
	GLuint id = 0;
	
	BASE_GL_FAIL_IF(n < 0, GL_ERROR_8000_DMP);
	
	for (i = 0; i < n; i++){
		newlist = (cl_list_t*)malloc(sizeof(cl_list_t));
		BASE_GL_FAIL_IF(!newlist, GL_ERROR_8001_DMP);
		memset(newlist, 0, sizeof(cl_list_t));

		newlist->run_mode = NN_GX_CMDLIST_SERIAL_RUN;
		
		id = __miniCLID;
		for (;; id++){
			if (__sysman.cl_list_table[id & CL_LIST_TABLE_SIZE] == 0){
				/* add all list to top */
				cmdlists[i] = id;
				newlist->id = id;
				newlist->next = 0;
				__sysman.cl_list_table[id & CL_LIST_TABLE_SIZE] = newlist;
				break;
			}
			else if (__sysman.cl_list_table[id & CL_LIST_TABLE_SIZE]->id == id){
				continue;
			}
			else if (__sysman.cl_list_table[id & CL_LIST_TABLE_SIZE]->id > id){ // CL List Size is 0x20?
				cmdlists[i] = id;
				newlist->id = id;
				newlist->next = __sysman.cl_list_table[id & CL_LIST_TABLE_SIZE];
				__sysman.cl_list_table[id & CL_LIST_TABLE_SIZE] = newlist;
				break;
			}
			else{
				int bCont, bBreak;
				bCont = bBreak = 0;
				for (cllist = __sysman.cl_list_table[id & CL_LIST_TABLE_SIZE]->next, tmp = __sysman.cl_list_table[id & CL_LIST_TABLE_SIZE]; cllist != 0; cllist = cllist->next){
					if (cllist->id == id){
						bCont = 1;
						break;
					}
					else if (cllist->id > id){
						cmdlists[i] = id;
						newlist->id = id;
						tmp->next = newlist;
						newlist->next = cllist;
						bBreak = 1;
						break;
					}
					tmp = cllist;
				}
				if (bBreak)
					break;
				if (bCont)
					continue;
				
				if (!cllist){
					tmp->next = newlist;
					newlist->id = id;
					newlist->next = 0;
					cmdlists[i] = id;
					break;
				}
				assert(0);
			}
		}
		__miniCLID = id + 1;
	}
	
	return;
}

void nngxDeleteCmdlists(GLsizei n, const GLuint* cmdlists){
	GLsizei i;
	cl_list_t* cllist;
	cl_list_t* tmp;
	
	BASE_GL_FAIL_IF(n < 0, GL_ERROR_8002_DMP);

	for (i = 0; i < n; i++){
		if (!cmdlists[i])
			continue;
		
		tmp = 0;
		for (cllist = __sysman.cl_list_table[cmdlists[i] & CL_LIST_TABLE_SIZE]; cllist != 0; cllist = cllist->next){
			if (cllist->id >= cmdlists[i])
				break;
			tmp = cllist;
		}
		if (cllist == 0 || cllist->id != cmdlists[i])
			continue;
		
		if (__sysman.run_cmdlist == cllist){
			BASE_GL_FAIL_IF_CONT(__sysman.is_running, GL_ERROR_8003_DMP);

			__sysman.run_cmdlist = 0;
			__sysman.start_run = GL_FALSE;
		}

		if (__sysman.bound_cmdlist == cllist){
			__cb_current_command_buffer = __cb_current_max_command_buffer = 0;
			__sysman.bound_cmdlist = 0;
		}

		if (cllist->command_buffer)
			free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_COMMANDBUFFER, cllist->id, cllist->command_buffer);
		
		if (cmdlists[i] < __miniCLID)
			__miniCLID = cmdlists[i];
		
		if (tmp == 0)
			__sysman.cl_list_table[cmdlists[i] & CL_LIST_TABLE_SIZE] = __sysman.cl_list_table[cmdlists[i] & CL_LIST_TABLE_SIZE]->next;
		else
			tmp->next = cllist->next;
		free(cllist);
	}
	
	return;
}

void nngxBindCmdlist(GLuint cmdlist){
	cl_list_t*		cllist;
	
	BASE_GL_FAIL_IF(__sysman.saving_cmdlist, GL_ERROR_8005_DMP);
	__GET_CL_LIST(cmdlist, cllist)
	
	if (cllist == 0 && cmdlist != 0){
		cl_list_t* tmp;
		cl_list_t* newlist = (cl_list_t*)malloc(sizeof(cl_list_t));
		BASE_GL_FAIL_IF(!newlist, GL_ERROR_8004_DMP);
		memset(newlist, 0, sizeof(cl_list_t));
		newlist->id = cmdlist;
		newlist->run_mode = NN_GX_CMDLIST_SERIAL_RUN;

		if (__sysman.cl_list_table[cmdlist & CL_LIST_TABLE_SIZE] == 0)
			__sysman.cl_list_table[cmdlist & CL_LIST_TABLE_SIZE] = newlist;
		else{
			if (__sysman.cl_list_table[cmdlist & CL_LIST_TABLE_SIZE]->id > cmdlist){
				newlist->next = __sysman.cl_list_table[cmdlist & CL_LIST_TABLE_SIZE];
				__sysman.cl_list_table[cmdlist & CL_LIST_TABLE_SIZE] = newlist;
			}
			else{
				for (cllist = __sysman.cl_list_table[cmdlist & CL_LIST_TABLE_SIZE]->next, tmp = __sysman.cl_list_table[cmdlist & CL_LIST_TABLE_SIZE]; cllist != 0; cllist = cllist->next){
					if (cllist->id > cmdlist){
						tmp->next = newlist;
						newlist->next = cllist;
						break;
					}
					tmp = cllist;
				}
				if (cllist == 0)
					tmp->next = newlist;
			}
		}
		cllist = newlist;
	}
	
	if (__sysman.bound_cmdlist != 0){
		__sysman.bound_cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(__sysman.bound_cmdlist->command_buffer);
	}
	
	__sysman.bound_cmdlist = cllist;

	if (cllist == 0 || cllist->command_buffer == 0)
		__cb_current_command_buffer = __cb_current_max_command_buffer = 0;
	else{
		__cb_current_command_buffer = (unsigned*)_PICA_MEM_NATIVE((unsigned char*)cllist->command_buffer + cllist->used_bufsize);
		__cb_current_max_command_buffer = (unsigned*)_PICA_MEM_NATIVE((unsigned char*)cllist->command_buffer + cllist->max_bufsize);
	}
	
	return;
}

void nngxCmdlistStorage(GLsizei bufsize, GLsizei requestcount){
	cl_list_t* cmdlist = __sysman.bound_cmdlist;
	unsigned char* newbuffer;
	if (cmdlist == 0)
		return;
	
	BASE_GL_FAIL_IF(cmdlist == __sysman.run_cmdlist && __sysman.is_running, GL_ERROR_8007_DMP);
	BASE_GL_FAIL_IF(bufsize < 0 || requestcount < 0, GL_ERROR_8008_DMP);

	if (cmdlist->command_buffer)
		free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_COMMANDBUFFER, cmdlist->id, cmdlist->command_buffer);

	(void*)newbuffer = malloc_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_COMMANDBUFFER, cmdlist->id, bufsize + requestcount * sizeof(cl_cmdreq_t));
	BASE_GL_FAIL_IF(newbuffer == 0 && (bufsize != 0 || requestcount != 0), GL_ERROR_8006_DMP);
	
	sys_CmdlistStorageCore(cmdlist, bufsize, newbuffer, requestcount, (void*)_PICA_MEM_NATIVE(newbuffer + bufsize));

	__cb_current_command_buffer = (unsigned*)_PICA_MEM_NATIVE(cmdlist->command_buffer);
	__cb_current_max_command_buffer = (unsigned*)_PICA_MEM_NATIVE((unsigned char*)cmdlist->command_buffer + cmdlist->max_bufsize);
	
	return;
}

void sys_CmdlistStorageCore(cl_list_t* cmdlist, GLsizei bufsize, GLvoid* commandbuffer, GLsizei requestcount, GLvoid* commandrequest){
	cmdlist->max_bufsize = bufsize;
	cmdlist->max_reqcount = requestcount;
	cmdlist->used_bufsize = 0;
	cmdlist->used_reqcount = 0;
	cmdlist->run_bufsize = 0;
	cmdlist->run_reqcount = 0;
	cmdlist->issued_reqcount = 0;
	cmdlist->subr_addr = 0;
	cmdlist->subr_bufsize = 0;
	cmdlist->command_buffer = (unsigned*)commandbuffer;
	cmdlist->command_request = (cl_cmdreq_t*)commandrequest;
	memset(cmdlist->command_request, 0, requestcount * sizeof(cl_cmdreq_t));
	
	return;
}

void nngxRunCmdlist(){
	sys_RunCmdlistCore(__sysman.bound_cmdlist);
	
	return;
}

void sys_RunCmdlistCore(cl_list_t* cmdlist){
	if (cmdlist == 0 || __sysman.is_running)
		return;
	
	BASE_GL_FAIL_IF(cmdlist->command_buffer == 0 || cmdlist->command_request == 0, GL_ERROR_8009_DMP);
	
	__sysman.run_cmdlist = cmdlist;
	
	__sysman.start_run = GL_TRUE;
	if (cmdlist->run_reqcount < cmdlist->used_reqcount){
		__sysman.is_running = GL_TRUE;
		switch (cmdlist->run_mode){
			case NN_GX_CMDLIST_SERIAL_RUN:
				nngxlowLock();
				sys_issueSpeculativeCommandRequests();
				nngxlowUnlock();
				break;
			case NN_GX_CMDLIST_PARALLEL_RUN:
			case NN_GX_CMDLIST_SYNC_RUN:
			default:
				break;
		}
	}
	else
		__sysman.is_running = GL_FALSE;
	
	return;
}

void nngxStopCmdlist(){
	cl_list_t* cmdlist;
	
	cmdlist = __sysman.run_cmdlist;
	if (cmdlist){
		nngxlowLock();
		if (__sysman.is_running){
			cmdlist->command_request[cmdlist->issued_reqcount - 1].stop_enabled = GL_TRUE;
			__sysman.stop_speculative = GL_TRUE;
		}
		else{
			__sysman.start_run = GL_FALSE;
		}
		nngxlowUnlock();
	}
	
	return;
}

void sys_issueSpeculativeCommandRequests(void){
	cl_list_t* cmdlist;

	if ( __sysman.stop_speculative == GL_TRUE )
		return;

	cmdlist = __sysman.run_cmdlist;

	while (cmdlist->issued_reqcount < cmdlist->used_reqcount){
		cl_cmdreq_t* cmdreq;
		if (cmdlist->issued_reqcount >= cmdlist->run_reqcount + __sysman.num_speculative){
			break;
		}
		cmdreq = &cmdlist->command_request[cmdlist->issued_reqcount];
		sys_executeCommandRequest(cmdreq);

		cmdlist->issued_reqcount ++;
		if (cmdreq->stop_enabled == GL_TRUE){
			__sysman.stop_speculative = GL_TRUE;
			break;
		}
	}
}

void __sys_markGasAcmMax(void){
	if (__sysman.bound_cmdlist){
		GLboolean gas_update = __sysman.bound_cmdlist->gas_accumulation;
		__sysman.bound_cmdlist->gas_accumulation = GL_TRUE;
		nngxSplitDrawCmdlist();

		if (!gas_update)
			__sysman.bound_cmdlist->gas_accumulation = GL_FALSE;
	}
	
	return;
}

void nngxSplitDrawCmdlist(){
	cl_cmdreq_t* cmdreq;
	cl_list_t* cmdlist = __sysman.bound_cmdlist;

	BASE_GL_FAIL_IF(cmdlist == 0, GL_ERROR_800C_DMP);
	BASE_GL_FAIL_IF(cmdlist->used_reqcount == cmdlist->max_reqcount, GL_ERROR_800D_DMP);

	cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(cmdlist->command_buffer);
	if (cmdlist->used_bufsize & 8){
		BASE_GL_FAIL_IF(cmdlist->max_bufsize < cmdlist->used_bufsize + 24, GL_ERROR_800E_DMP);
		cmdlist->used_bufsize += 24;
	}
	else{
		BASE_GL_FAIL_IF(cmdlist->max_bufsize < cmdlist->used_bufsize + 32, GL_ERROR_800E_DMP);
		cmdlist->used_bufsize += 32;
		__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
	}

	__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf); 
	__cb_writeReg(PA_WO_FU_TAG_CLEAR, 1, 0xf); 
	__cb_writeReg(PA_RW_SYS_FRONT_REG0, PICA_3D_INTR_CODE, 0xf);

	cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
	
	cmdreq->id = CL_CMDREQ_ID_REN;
	cmdreq->param.ren.addr = (unsigned)cmdlist->command_buffer + cmdlist->last_split_offset;
	cmdreq->param.ren.size = cmdlist->used_bufsize - cmdlist->last_split_offset;
	if (cmdlist->subr_addr)
		sys_fixSubroutineCommand(cmdlist, cmdreq);
	cmdreq->param.ren.extbuf = 0;
	if (cmdlist->gas_accumulation)
		cmdreq->param.ren.gas = 1;
	cmdlist->last_split_offset = cmdlist->used_bufsize;

	nngxlowLock();
	cmdlist->used_reqcount++;
	if (cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running){
		__sysman.is_running = GL_TRUE;
		sys_issueSpeculativeCommandRequests();
	}
	nngxlowUnlock();
	
	return;
}

void nngxFlush3DCommand(){
	cl_cmdreq_t* cmdreq;
	cl_list_t* cmdlist = __sysman.bound_cmdlist;

	BASE_GL_FAIL_IF(cmdlist == 0, GL_ERROR_8084_DMP);
	BASE_GL_FAIL_IF(cmdlist->used_reqcount == cmdlist->max_reqcount, GL_ERROR_8085_DMP);

	cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(cmdlist->command_buffer);
	if (cmdlist->last_split_offset != cmdlist->used_bufsize){
		if (cmdlist->used_bufsize & 8){
			BASE_GL_FAIL_IF(cmdlist->max_bufsize < cmdlist->used_bufsize + 24, GL_ERROR_8086_DMP);
			cmdlist->used_bufsize += 24;
		}
		else{
			BASE_GL_FAIL_IF(cmdlist->max_bufsize < cmdlist->used_bufsize + 32, GL_ERROR_8086_DMP);
			cmdlist->used_bufsize += 32;
			__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
		}

		__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf); 
		__cb_writeReg(PA_WO_FU_TAG_CLEAR, 1, 0xf); 
		__cb_writeReg(PA_RW_SYS_FRONT_REG0, PICA_3D_INTR_CODE, 0xf);

		cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
		
		cmdreq->id = CL_CMDREQ_ID_REN;
		cmdreq->param.ren.addr = (unsigned)cmdlist->command_buffer + cmdlist->last_split_offset;
		cmdreq->param.ren.size = cmdlist->used_bufsize - cmdlist->last_split_offset;
		if (cmdlist->subr_addr)
			sys_fixSubroutineCommand(cmdlist, cmdreq);
		cmdreq->param.ren.extbuf = 0;
		if (cmdlist->gas_accumulation)
			cmdreq->param.ren.gas = 1;
		cmdlist->last_split_offset = cmdlist->used_bufsize;
		
		nngxlowLock();
		cmdlist->used_reqcount++;
		if (cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running){
			__sysman.is_running = GL_TRUE;
			sys_issueSpeculativeCommandRequests();
		}
		nngxlowUnlock();
	}
}

void nngxClearCmdlist(){
	cl_list_t* cmdlist = __sysman.bound_cmdlist;
	if (cmdlist == 0 || (cmdlist->command_buffer == 0 && cmdlist->command_request == 0))
		return;
	
	BASE_GL_FAIL_IF(cmdlist == __sysman.run_cmdlist && __sysman.is_running, GL_ERROR_800F_DMP);
	
	sys_ClearCmdlistCore(cmdlist);
	
	__cb_current_command_buffer = (unsigned*)_PICA_MEM_NATIVE((unsigned char*)cmdlist->command_buffer);
	__cb_current_max_command_buffer = (unsigned*)_PICA_MEM_NATIVE((unsigned char*)cmdlist->command_buffer + cmdlist->max_bufsize);
}

void sys_ClearCmdlistCore(cl_list_t* cmdlist){
	memset(cmdlist->command_request, 0, sizeof(cl_cmdreq_t) * cmdlist->used_reqcount);
	cmdlist->used_bufsize = 0;
	cmdlist->run_bufsize = 0;
	cmdlist->used_reqcount = 0;
	cmdlist->run_reqcount = 0;
	cmdlist->last_split_offset = 0;
	cmdlist->issued_reqcount = 0;
	cmdlist->subr_addr = 0;
	cmdlist->subr_bufsize = 0;
	
	return;
}

void sys_SetCmdlistCallbackCore(cl_list_t* cmdlist, void (*func)(GLint)){
	if (cmdlist == 0)
		return;
	BASE_GL_FAIL_IF(cmdlist == __sysman.run_cmdlist && __sysman.is_running, GL_ERROR_8010_DMP);
	
	cmdlist->callbackfunc = func;
}

void nngxEnableCmdlistCallback(GLint id){
	sys_EnableCmdlistCallbackCore(__sysman.bound_cmdlist, id);
}

void sys_EnableCmdlistCallbackCore(cl_list_t* cmdlist, GLint id){
	if (cmdlist == 0)
		return;
	
	if (id == -1)
		cmdlist->last_callback_enabled = GL_TRUE;
	else{
		BASE_GL_FAIL_IF(cmdlist->max_reqcount < id || id == 0 || id < 0, GL_ERROR_8012_DMP);
		cmdlist->command_request[id - 1].callback_enabled = GL_TRUE;
	}
}

void nngxSetCmdlistParameteri(GLenum pname, GLint param){
	cl_list_t* cmdlist = __sysman.bound_cmdlist;
	if (cmdlist == 0)
		return;
	BASE_GL_FAIL_IF(cmdlist == __sysman.run_cmdlist && __sysman.is_running, GL_ERROR_8015_DMP);
	
	switch (pname){
		case NN_GX_CMDLIST_RUN_MODE:
			BASE_GL_FAIL_IF(param != NN_GX_CMDLIST_SERIAL_RUN, GL_ERROR_8016_DMP);
			cmdlist->run_mode = param;
			break;
		case NN_GX_CMDLIST_GAS_UPDATE:
			cmdlist->gas_accumulation = (param != 0) ? GL_TRUE : GL_FALSE;
			break;
		default:
			__err_setError(GL_ERROR_8016_DMP);
			break;
	}
	
	return;
}

void nngxGetCmdlistParameteri(GLenum pname, GLint* param){
	cl_list_t* cmdlist = __sysman.bound_cmdlist;
	BASE_GL_FAIL_IF(cmdlist == 0 && pname != NN_GX_CMDLIST_BINDING && pname != NN_GX_CMDLIST_CURRENT_BUFADDR
		&& pname != NN_GX_CMDLIST_HW_STATE, GL_ERROR_8018_DMP);
	
	switch (pname){
		case NN_GX_CMDLIST_RUN_MODE:
			param[0] = cmdlist->run_mode;
			break;
		case NN_GX_CMDLIST_IS_RUNNING:
			param[0] = (cmdlist == __sysman.run_cmdlist && __sysman.is_running) ? GL_TRUE : GL_FALSE;
			break;
		case NN_GX_CMDLIST_USED_BUFSIZE:
			cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(cmdlist->command_buffer);
			param[0] = cmdlist->used_bufsize;
			break;
		case NN_GX_CMDLIST_USED_REQCOUNT:
			param[0] = cmdlist->used_reqcount;
			break;
		case NN_GX_CMDLIST_MAX_BUFSIZE:
			param[0] = cmdlist->max_bufsize;
			break;
		case NN_GX_CMDLIST_MAX_REQCOUNT:
			param[0] = cmdlist->max_reqcount;
			break;
		case NN_GX_CMDLIST_TOP_BUFADDR:
			param[0] = (GLint)cmdlist->command_buffer;
			break;
		case NN_GX_CMDLIST_BINDING:
		    param[0] = __sysman.bound_cmdlist == 0 ? 0 : __sysman.bound_cmdlist->id;
		    break;
		case NN_GX_CMDLIST_CURRENT_BUFADDR:
			param[0] = (GLint)__cb_current_command_buffer;
			break;
		case NN_GX_CMDLIST_RUN_BUFSIZE:
			param[0] = cmdlist->run_bufsize;
			break;
		case NN_GX_CMDLIST_RUN_REQCOUNT:
			param[0] = cmdlist->run_reqcount;
			break;
		case NN_GX_CMDLIST_TOP_REQADDR:
			param[0] = (GLint)cmdlist->command_request;
			break;
		case NN_GX_CMDLIST_NEXT_REQTYPE:
			if (cmdlist->used_reqcount > cmdlist->run_reqcount){
				switch (cmdlist->command_request[cmdlist->run_reqcount].id){
					case CL_CMDREQ_ID_DMA:
						param[0] = NN_GX_CMDLIST_REQTYPE_DMA;
						break;
					case CL_CMDREQ_ID_REN:
						param[0] = NN_GX_CMDLIST_REQTYPE_RUN3D;
						break;
					case CL_CMDREQ_ID_MF:
						param[0] = NN_GX_CMDLIST_REQTYPE_FILLMEM;
						break;
					case CL_CMDREQ_ID_PF:
						param[0] = NN_GX_CMDLIST_REQTYPE_POSTTRANS;
						break;
					case CL_CMDREQ_ID_CTX:
						param[0] = NN_GX_CMDLIST_REQTYPE_COPYTEX;
						break;
				}
			}
			break;
		case NN_GX_CMDLIST_NEXT_REQINFO:
			if (cmdlist->used_reqcount > cmdlist->run_reqcount){
				switch (cmdlist->command_request[cmdlist->run_reqcount].id){
					case CL_CMDREQ_ID_REN:
						param[0] = cmdlist->command_request[cmdlist->run_reqcount].param.ren.addr;
						param[1] = cmdlist->command_request[cmdlist->run_reqcount].param.ren.size;
						break;
				}
			}
			break;
		case NN_GX_CMDLIST_HW_STATE:
			nngxlowReadHWRegs(GARNET_ADDR_PSC(GARNET_REG_PSC_DEBUG), (void*)param, 4);
			break;
		default:
			__err_setError(GL_ERROR_8017_DMP);
			break;
	}
	
	return;
}

GLboolean nngxGetIsRunning(void){
	return __sysman.is_running;
}

void nngxWaitCmdlistDone(void){
	s64 starttime = 0;
	
	if (__sysman.timeout)
		starttime = nngxlowGetSystemTick();
	
	for(;;){
		if (!__sysman.is_running)
			break;
		
		if (__sysman.timeout){
			if (__sysman.timeout < (nngxlowGetSystemTick() - starttime)){
				if (__sysman.tocallback)
					__sysman.tocallback();
				break;
			}
		}
		
		nngxlowYieldThread();
	}
	return;
}

void nngxSetTimeout(s64 time, void (*callback)(void)){
	__sysman.timeout = time;
	__sysman.tocallback = callback;

	return;
}

void __sys_setDMACommandRequest(void* dstaddr, void* srcaddr, unsigned size){
	cl_cmdreq_t* cmdreq;
	cl_list_t* cmdlist = __sysman.bound_cmdlist;
	
	BASE_GL_FAIL_IF(cmdlist == 0 || cmdlist->used_reqcount == cmdlist->max_reqcount, GL_ERROR_COMMANDREQUEST_FULL_DMP);
	
	cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
	
	cmdreq->id = CL_CMDREQ_ID_DMA;
	cmdreq->param.dma.srcaddr = (unsigned)srcaddr;
	cmdreq->param.dma.dstaddr = (unsigned)dstaddr;
	cmdreq->param.dma.size = size;

	nngxlowLock();
	cmdlist->used_reqcount++;
	if (cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running){
		__sysman.is_running = GL_TRUE;
		sys_issueSpeculativeCommandRequests();
	}
	nngxlowUnlock();
	
	return;
}

static void sys_executeCommandRequest(cl_cmdreq_t* cmdreq){
	switch (cmdreq->id){
		case CL_CMDREQ_ID_DMA:{
				GLboolean flushCache = cmdreq->param.dma.noflush ? GL_FALSE : GL_TRUE;
				nngxlowRequestDMAEx((void*)cmdreq->param.dma.dstaddr, (const void*)cmdreq->param.dma.srcaddr, (GLuint)cmdreq->param.dma.size, flushCache);
			}
			break;
		case CL_CMDREQ_ID_REN:{
				GLboolean  gas = cmdreq->param.ren.gas ? GL_FALSE : GL_TRUE;
				GLboolean flushCache = cmdreq->param.ren.noflush ? GL_FALSE : GL_TRUE;
				nngxlowSetCommandlistEx((void*)cmdreq->param.ren.addr, cmdreq->param.ren.size, flushCache, gas);
			}
			break;
		case CL_CMDREQ_ID_MF:{
				nngxlowSetMemoryFill((void*)cmdreq->param.mf.start0, (void*)(cmdreq->param.mf.start0 + cmdreq->param.mf.size0), cmdreq->param.mf.data0, 0x00000001 | (cmdreq->param.mf.format0 << 8),
									(void*)cmdreq->param.mf.start1, (void*)(cmdreq->param.mf.start1 + cmdreq->param.mf.size1), cmdreq->param.mf.data1, 0x00000001 | (cmdreq->param.mf.format1 << 8));
			}
			break;
		case CL_CMDREQ_ID_PF:{
				unsigned mode = 0;

				((garnet_ppf_reg_mode_t*)&mode)->yflip = cmdreq->param.pf.yflip;
				((garnet_ppf_reg_mode_t*)&mode)->crop = (cmdreq->param.pf.dwidth < cmdreq->param.pf.swidth || cmdreq->param.pf.dheight < cmdreq->param.pf.sheight) ? 1 : 0;
				((garnet_ppf_reg_mode_t*)&mode)->sformat = cmdreq->param.pf.sformat;
				((garnet_ppf_reg_mode_t*)&mode)->dformat = cmdreq->param.pf.dformat;
				((garnet_ppf_reg_mode_t*)&mode)->amode = cmdreq->param.pf.amode;
				((garnet_ppf_reg_mode_t*)&mode)->blk32 = cmdreq->param.pf.blk32;
				((garnet_ppf_reg_mode_t*)&mode)->l2b = cmdreq->param.pf.l2b;
				((garnet_ppf_reg_mode_t*)&mode)->b2b = cmdreq->param.pf.b2b;
				
				nngxlowSetDisplayTransfer((void*)cmdreq->param.pf.srcaddr, cmdreq->param.pf.swidth, cmdreq->param.pf.sheight,
											(void*)cmdreq->param.pf.dstaddr, cmdreq->param.pf.dwidth, cmdreq->param.pf.dheight, mode);
			}
			break;
		case CL_CMDREQ_ID_CTX:{
				unsigned mode = 0;

				((garnet_ppf_reg_mode_t*)&mode)->crop = (cmdreq->param.ctx.sintiv || cmdreq->param.ctx.dintiv) ? 1 : 0;
				((garnet_ppf_reg_mode_t*)&mode)->dmac = 1;

				nngxlowSetTextureCopy((void*)cmdreq->param.ctx.srcaddr, (void*)cmdreq->param.ctx.dstaddr, cmdreq->param.ctx.size,
							cmdreq->param.ctx.sintv, cmdreq->param.ctx.sintiv, cmdreq->param.ctx.dintv, cmdreq->param.ctx.dintiv, mode);
			}
			break;
	}
	
	return;
}

static void sys_P3DCallback(){
	cl_cmdreq_t* cmdreq = &__sysman.run_cmdlist->command_request[__sysman.run_cmdlist->run_reqcount];

	if (!cmdreq->param.ren.extbuf)
		__sysman.run_cmdlist->run_bufsize += cmdreq->param.ren.size;
	
	sys_commonIntrHandler();
	
	return;
}

static void sys_PSC0Callback(){
	sys_commonIntrHandler();
}

static void sys_PSC1Callback(){
	sys_commonIntrHandler();
}

static void sys_PPFCallback(){	
	sys_commonIntrHandler();
}

static void sys_DMACallback(){
	sys_commonIntrHandler();
}

static void sys_PDC0Callback(){
	if (__sysman.v0callback)
		__sysman.v0callback(NN_GX_DISPLAY0);

	__sysman.v_counter[0] = (__sysman.v_counter[0] + 1) % (V_COUNTER_LIMIT + 1);
}

static void sys_PDC1Callback(){
	if (__sysman.v1callback)
		__sysman.v1callback(NN_GX_DISPLAY1);

	__sysman.v_counter[1] = (__sysman.v_counter[1] + 1) % (V_COUNTER_LIMIT + 1);
}

static void sys_commonIntrHandler(void){
	cl_cmdreq_t* cmdreq = &__sysman.run_cmdlist->command_request[__sysman.run_cmdlist->run_reqcount];
	cl_list_t* cmdlist = __sysman.run_cmdlist;
	void (*callbackfunc)(GLint) = 0;
	GLint callbackarg;
	
	nngxlowLock();

	cmdlist->run_reqcount++;

	if (cmdlist->callbackfunc && (cmdreq->callback_enabled || (cmdlist->run_reqcount == cmdlist->used_reqcount && cmdlist->last_callback_enabled))){
		callbackfunc = cmdlist->callbackfunc;
		callbackarg = cmdlist->run_reqcount;
	}

	if (cmdreq->stop_enabled == GL_FALSE){

		if (cmdlist->run_reqcount < cmdlist->used_reqcount){
			sys_issueSpeculativeCommandRequests();
		}
		else
			__sysman.is_running = GL_FALSE;
	}
	else{
		__sysman.start_run = GL_FALSE;
		__sysman.stop_speculative = GL_FALSE;
		__sysman.is_running = GL_FALSE;
	}
	nngxlowUnlock();

	if (callbackfunc)
		callbackfunc(callbackarg);
	
	return;
}

GLint nngxCheckVSync(GLenum display){
	GLint ret = 0;
	
	switch (display){
		case NN_GX_DISPLAY0:
			ret = __sysman.v_counter[0];
			break;
		case NN_GX_DISPLAY1:
			ret = __sysman.v_counter[1];
			break;
		case NN_GX_DISPLAY_BOTH:
			if (__sysman.v_counter[0] == __sysman.v_counter[1])
				ret = __sysman.v_counter[0];
			else{
				if ((__sysman.v_counter[0] == V_COUNTER_LIMIT && __sysman.v_counter[1] == 0)
					|| (__sysman.v_counter[1] == V_COUNTER_LIMIT && __sysman.v_counter[0] == 0))
					ret = V_COUNTER_LIMIT;
				else if (__sysman.v_counter[0] < __sysman.v_counter[1])
					ret = __sysman.v_counter[0];
				else
					ret = __sysman.v_counter[1];
			}
			break;
		default:
			__err_setError(GL_ERROR_8019_DMP);
			break;
	}
	
	return ret;
}

void nngxWaitVSync(GLenum display){
	GLint v_counter[2];
	
	BASE_GL_FAIL_IF(display != NN_GX_DISPLAY0 && display != NN_GX_DISPLAY1 && display != NN_GX_DISPLAY_BOTH, GL_ERROR_801A_DMP);
	
	v_counter[0] = __sysman.v_counter[0];
	v_counter[1] = __sysman.v_counter[1];
	
	for(;;){
		int vsync = 0;
		
		switch (display){
			case NN_GX_DISPLAY0:
				if (v_counter[0] != __sysman.v_counter[0])
					vsync = 1;
				break;
			case NN_GX_DISPLAY1:
				if (v_counter[1] != __sysman.v_counter[1])
					vsync = 1;
				break;
			case NN_GX_DISPLAY_BOTH:
				if (v_counter[0] != __sysman.v_counter[0] && v_counter[1] != __sysman.v_counter[1])
					vsync = 1;
				break;
		}
		
		if (vsync)
			break;
		nngxlowYieldThread();
	}
	
	return;
}

void nngxGenDisplaybuffers(GLsizei n, GLuint* buffers){
	GLint i;
	db_list_t* dblist;
	db_list_t* tmp;
	db_list_t* newlist;
	GLuint id = 0;
	
	BASE_GL_FAIL_IF(n < 0, GL_ERROR_801C_DMP);
	
	for (i = 0; i < n; i++){
		newlist = (db_list_t*)malloc(sizeof(db_list_t));
		BASE_GL_FAIL_IF(!newlist, GL_ERROR_801D_DMP);
		memset(newlist, 0, sizeof(db_list_t));
		
		id = __miniDBID;
		for ( ; ; id++){
			if (__sysman.db_list_table[id & DB_LIST_TABLE_SIZE] == 0){
				buffers[i] = id;
				newlist->id = id;
				newlist->next = 0;
				__sysman.db_list_table[id & DB_LIST_TABLE_SIZE] = newlist;
				break;
			}
			else if (__sysman.db_list_table[id & DB_LIST_TABLE_SIZE]->id == id){
				continue;
			}
			else if (__sysman.db_list_table[id & DB_LIST_TABLE_SIZE]->id > id){
				buffers[i] = id;
				newlist->id = id;
				newlist->next = __sysman.db_list_table[id & DB_LIST_TABLE_SIZE];
				__sysman.db_list_table[id & DB_LIST_TABLE_SIZE] = newlist;
				break;
			}
			else{
				int bCont, bBreak;
				bCont = bBreak = 0;
				for (dblist = __sysman.db_list_table[id & DB_LIST_TABLE_SIZE]->next, tmp = __sysman.db_list_table[id & DB_LIST_TABLE_SIZE]; dblist != 0; dblist = dblist->next){
					if (dblist->id == id){
						bCont = 1;
						break;
					}
					else if (dblist->id > id){
						buffers[i] = id;
						newlist->id = id;
						tmp->next = newlist;
						newlist->next = dblist;
						bBreak = 1;
						break;
					}
					tmp = dblist;
				}
				if (bBreak)
					break;
				if (bCont)
					continue;
				
				if (!dblist){
					tmp->next = newlist;
					newlist->id = id;
					newlist->next = 0;
					buffers[i] = id;
					break;
				}
				assert(0);
			}
		}
		__miniDBID = id + 1;
	}
	
	return;
}

void nngxDeleteDisplaybuffers(GLsizei n, GLuint* buffers)
{
	GLsizei i;
	db_list_t* dblist;
	db_list_t* tmp;
	
	BASE_GL_FAIL_IF(n < 0, GL_ERROR_801E_DMP);

	for (i = 0; i < n; i++){
		if (!buffers[i])
			continue;
		
		tmp = 0;
		for (dblist = __sysman.db_list_table[buffers[i] & DB_LIST_TABLE_SIZE]; dblist != 0; dblist = dblist->next){
			if (dblist->id >= buffers[i])
				break;
			tmp = dblist;
		}
		if (dblist == 0 || dblist->id != buffers[i])
			continue;

		if (dblist->address)
			free_ext(dblist->area, NN_GX_MEM_DISPLAYBUFFER, dblist->id, dblist->address);
		
		if (buffers[i] < __miniDBID)
			__miniDBID = buffers[i];
		
		if (tmp == 0)
			__sysman.db_list_table[buffers[i] & DB_LIST_TABLE_SIZE] = __sysman.db_list_table[buffers[i] & DB_LIST_TABLE_SIZE]->next;
		else
			tmp->next = dblist->next;
		if (__sysman.current_buffer[0] == dblist)
			__sysman.current_buffer[0] = 0;
		if (__sysman.current_buffer[1] == dblist)
			__sysman.current_buffer[1] = 0;
		if (__sysman.current_buffer[DISP0_EXT_BINDING_POINT] == dblist)
			__sysman.current_buffer[DISP0_EXT_BINDING_POINT] = 0;
		free(dblist);
	}
	
	return;
}

void nngxActiveDisplay(GLenum display){
	switch (display){
		case NN_GX_DISPLAY0:
		case NN_GX_DISPLAY1:
			__sysman.active_display = display - NN_GX_DISPLAY0;
			break;
		case NN_GX_DISPLAY0_EXT:
			__sysman.active_display = DISP0_EXT_BINDING_POINT;
			break;
		default:
			__err_setError(GL_ERROR_801F_DMP);
			break;
	}
	
	return;
}

void nngxBindDisplaybuffer(GLuint buffer)
{
	db_list_t* dblist;
	
	__GET_DB_LIST(buffer, dblist)
	
	if (buffer != 0 && dblist == 0){
		db_list_t* tmp;
		db_list_t* newlist = (db_list_t*)malloc(sizeof(db_list_t));
		BASE_GL_FAIL_IF(!newlist, GL_ERROR_8020_DMP);
		memset(newlist, 0, sizeof(db_list_t));
		newlist->id = buffer;

		if (__sysman.db_list_table[buffer & DB_LIST_TABLE_SIZE] == 0)
			__sysman.db_list_table[buffer & DB_LIST_TABLE_SIZE] = newlist;
		else{
			if (__sysman.db_list_table[buffer & DB_LIST_TABLE_SIZE]->id > buffer){
				newlist->next = __sysman.db_list_table[buffer & DB_LIST_TABLE_SIZE];
				__sysman.db_list_table[buffer & DB_LIST_TABLE_SIZE] = newlist;
			}
			else{
				for (dblist = __sysman.db_list_table[buffer & DB_LIST_TABLE_SIZE]->next, tmp = __sysman.db_list_table[buffer & DB_LIST_TABLE_SIZE]; dblist != 0; dblist = dblist->next){
					if (dblist->id > buffer){

						tmp->next = newlist;
						newlist->next = dblist;
						break;
					}
					tmp = dblist;
				}
				if (dblist == 0)
					tmp->next = newlist;
			}
		}
		dblist = newlist;
	}
	__sysman.current_buffer[__sysman.active_display] = dblist;
	
	return;
}

void nngxDisplaybufferStorage(GLenum format, GLsizei width, GLsizei height, GLenum area){
	int pixelsize;
	db_list_t* dblist = __sysman.current_buffer[__sysman.active_display];
	
	BASE_GL_FAIL_IF(dblist == 0, GL_ERROR_8021_DMP);
	BASE_GL_FAIL_IF((width & 0x7) || (height & 0x7) || (width <= 0) || (height <= 0), GL_ERROR_8022_DMP);
	
	switch (format){
		case GL_RGBA8_OES:	pixelsize = 4;	break;
		case GL_RGB8_OES:	pixelsize = 3;	break;
		case GL_RGBA4:		pixelsize = 2;	break;
		case GL_RGB5_A1:	pixelsize = 2;	break;
		case GL_RGB565:		pixelsize = 2;	break;
		default:
			__err_setError(GL_ERROR_8023_DMP);
			return;
	}
	switch (area){
		case NN_GX_MEM_FCRAM:
		case NN_GX_MEM_VRAMA:
		case NN_GX_MEM_VRAMB:
			break;
		default:
			__err_setError(GL_ERROR_8024_DMP);
			return;
	}
	
	if (dblist->address)
		free_ext(dblist->area, NN_GX_MEM_DISPLAYBUFFER, dblist->id, dblist->address);
	dblist->address = malloc_ext(area, NN_GX_MEM_DISPLAYBUFFER, dblist->id, pixelsize * width * height);
	BASE_GL_FAIL_IF(dblist->address == 0, GL_ERROR_8025_DMP);
	
	dblist->format = format;
	dblist->width = width;
	dblist->height = height;
	dblist->area = area;
	
	return;
}

void nngxDisplayEnv(GLint displayx, GLint displayy){
	BASE_GL_FAIL_IF((displayx < 0) || (displayy < 0), GL_ERROR_8026_DMP);
	
	__sysman.display_xoffset[__sysman.active_display] = displayx;
	__sysman.display_yoffset[__sysman.active_display] = displayy;
	
	return;
}

void nngxTransferRenderImage(GLuint buffer, GLenum mode, GLboolean yflip, GLint colorx, GLint colory){
	cl_cmdreq_t* cmdreq;
	cl_list_t* cmdlist = __sysman.bound_cmdlist;
	db_list_t* dblist;
	unsigned amode;
	int pixelsize;
	int xscale, yscale;
	srf_container_t* pSurfColor;
	int blocksize;
	GET_CURRENT_STATE(state);
	
	blocksize = state->gl_state.fb_state.block32Enabled ? 32 : 8;
	
	__GET_DB_LIST(buffer, dblist)
	pSurfColor = __fb_getCurrentFBColorSurface();

	BASE_GL_FAIL_IF(cmdlist == 0, GL_ERROR_8027_DMP);
	BASE_GL_FAIL_IF(cmdlist->used_reqcount == cmdlist->max_reqcount, GL_ERROR_8028_DMP);
	BASE_GL_FAIL_IF(dblist == 0 || dblist->address == 0, GL_ERROR_8029_DMP);
	BASE_GL_FAIL_IF(pSurfColor == 0 || pSurfColor->picaaddr == 0, GL_ERROR_802A_DMP);
	BASE_GL_FAIL_IF(pSurfColor->pixelSize == 16 && (dblist->format == GL_RGBA8_OES || dblist->format == GL_RGB8_OES), GL_ERROR_802E_DMP);
	switch (mode){
		case NN_GX_ANTIALIASE_NOT_USED:	amode = GARNET_PPF_AMODE_NOP; xscale = 1; yscale = 1; break;
		case NN_GX_ANTIALIASE_2x1:		amode = GARNET_PPF_AMODE_2x1; xscale = 2; yscale = 1; break;
		case NN_GX_ANTIALIASE_2x2:		amode = GARNET_PPF_AMODE_2x2; xscale = 2; yscale = 2; break;
		default:
			__err_setError(GL_ERROR_802B_DMP);
			return;
	}
	BASE_GL_FAIL_IF((pSurfColor->width - colorx) < (dblist->width * xscale) || (pSurfColor->height - colory) < (dblist->height * yscale), GL_ERROR_802C_DMP);
	BASE_GL_FAIL_IF((colorx & (blocksize - 1)) || (colory & (blocksize - 1)) || (colorx < 0) || (colory < 0), GL_ERROR_802D_DMP);
	BASE_GL_FAIL_IF((pSurfColor->width & (blocksize - 1)) || (pSurfColor->height & (blocksize - 1))
				|| (dblist->width & (blocksize - 1)) || (dblist->height & (blocksize - 1)), GL_ERROR_8059_DMP);
	BASE_GL_FAIL_IF(dblist->format == GL_RGB8_OES && ((pSurfColor->width & 0xf) || (dblist->width & 0xf)), GL_ERROR_805A_DMP);
	
	cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(cmdlist->command_buffer);
	if (cmdlist->last_split_offset != cmdlist->used_bufsize){
		if (cmdlist->used_bufsize & 8){
			BASE_GL_FAIL_IF(cmdlist->max_bufsize < cmdlist->used_bufsize + 24, GL_ERROR_802F_DMP);
			cmdlist->used_bufsize += 24;
		}
		else{
			BASE_GL_FAIL_IF(cmdlist->max_bufsize < cmdlist->used_bufsize + 32, GL_ERROR_802F_DMP);

			cmdlist->used_bufsize += 32;
			__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
		}
		__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf); 
		__cb_writeReg(PA_WO_FU_TAG_CLEAR, 1, 0xf); 
		__cb_writeReg(PA_RW_SYS_FRONT_REG0, PICA_3D_INTR_CODE, 0xf);

		cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
		
		cmdreq->id = CL_CMDREQ_ID_REN;
		cmdreq->param.ren.addr = (unsigned)cmdlist->command_buffer + cmdlist->last_split_offset;
		cmdreq->param.ren.size = cmdlist->used_bufsize - cmdlist->last_split_offset;
		if (cmdlist->subr_addr)
			sys_fixSubroutineCommand(cmdlist, cmdreq);
		cmdreq->param.ren.extbuf = 0;
		cmdlist->last_split_offset = cmdlist->used_bufsize;
		cmdlist->used_reqcount++;
		BASE_GL_FAIL_IF(cmdlist->used_reqcount == cmdlist->max_reqcount, GL_ERROR_8028_DMP);
	}

	cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
	
	cmdreq->id = CL_CMDREQ_ID_PF;
	switch (pSurfColor->residentType){
		case TF_R8G8B8A8:	pixelsize = 4;	cmdreq->param.pf.sformat = GARNET_PPF_FORMAT_R8G8B8A8;	break;
		case TF_R8G8B8:		pixelsize = 3;	cmdreq->param.pf.sformat = GARNET_PPF_FORMAT_R8G8B8;	break;
		case TF_R5G6B5:		pixelsize = 2;	cmdreq->param.pf.sformat = GARNET_PPF_FORMAT_R5G6B5;	break;
		case TF_R5G5B5A1:	pixelsize = 2;	cmdreq->param.pf.sformat = GARNET_PPF_FORMAT_R5G5B5A1;	break;
		case TF_R4G4B4A4:	pixelsize = 2;	cmdreq->param.pf.sformat = GARNET_PPF_FORMAT_R4G4B4A4;	break;
		default: pixelsize = 0; break;
	}
	cmdreq->param.pf.srcaddr = (unsigned)pSurfColor->picaaddr + 
		(pSurfColor->width * (pSurfColor->height - dblist->height * yscale - colory) + colorx * blocksize) * pixelsize;
	switch (dblist->format){
		case GL_RGBA8_OES:	pixelsize = 4;	cmdreq->param.pf.dformat = GARNET_PPF_FORMAT_R8G8B8A8;	break;
		case GL_RGB8_OES:	pixelsize = 3;	cmdreq->param.pf.dformat = GARNET_PPF_FORMAT_R8G8B8;	break;
		case GL_RGB565:		pixelsize = 2;	cmdreq->param.pf.dformat = GARNET_PPF_FORMAT_R5G6B5;	break;
		case GL_RGB5_A1:	pixelsize = 2;	cmdreq->param.pf.dformat = GARNET_PPF_FORMAT_R5G5B5A1;	break;
		case GL_RGBA4:		pixelsize = 2;	cmdreq->param.pf.dformat = GARNET_PPF_FORMAT_R4G4B4A4;	break;
		default: pixelsize = 0; break;
	}
	cmdreq->param.pf.dstaddr = (unsigned)dblist->address;
	cmdreq->param.pf.swidth = pSurfColor->width;
	cmdreq->param.pf.sheight = pSurfColor->height;
	cmdreq->param.pf.dwidth = dblist->width * xscale;
	cmdreq->param.pf.dheight = dblist->height * yscale;
	if (yflip && (cmdreq->param.pf.dwidth < cmdreq->param.pf.swidth || cmdreq->param.pf.dheight < cmdreq->param.pf.sheight)){
		if (xscale == 2){

			int minlen = pixelsize == 3 ? 16 : 8;
			int diffwidth = cmdreq->param.pf.swidth - cmdreq->param.pf.dwidth;
			
			if (diffwidth & minlen)
				diffwidth -= minlen;
			
			cmdreq->param.pf.dstaddr -= diffwidth * (dblist->height - 1) * pixelsize / 2;
		}
		else
			cmdreq->param.pf.dstaddr -= (cmdreq->param.pf.swidth - cmdreq->param.pf.dwidth) * (cmdreq->param.pf.dheight - 1) * pixelsize;
	}
	cmdreq->param.pf.yflip = yflip ? 1 : 0;
	cmdreq->param.pf.amode = amode;
	cmdreq->param.pf.blk32 = state->gl_state.fb_state.block32Enabled ? 1 : 0;
	cmdreq->param.pf.l2b = 0;
	cmdreq->param.pf.b2b = 0;

	nngxlowLock();
	cmdlist->used_reqcount++;
	if (cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running)
	{
		__sysman.is_running = GL_TRUE;
		sys_issueSpeculativeCommandRequests();
	}
	nngxlowUnlock();
	
	return;
}

void nngxSwapBuffers(GLenum display){
	int i;
	
	BASE_GL_FAIL_IF(display != NN_GX_DISPLAY0 && display != NN_GX_DISPLAY1 && display != NN_GX_DISPLAY_BOTH, GL_ERROR_8030_DMP);
	for (i = 0; i < 2; i++){
		db_list_t* dblist;
		unsigned pixelsize;
		unsigned format;
		unsigned burstlen;
		unsigned addr, addrB = 0;
		unsigned mode;
		unsigned size;
		
		if (display != NN_GX_DISPLAY0 + i && display != NN_GX_DISPLAY_BOTH)
			continue;
		
		dblist = __sysman.current_buffer[i];
		BASE_GL_FAIL_IF_CONT(dblist == 0 || dblist->address == 0, GL_ERROR_8031_DMP);
		BASE_GL_FAIL_IF_CONT(dblist->height <= __sysman.display_yoffset[i] || dblist->width <= __sysman.display_xoffset[i], GL_ERROR_8032_DMP);
		
		switch (dblist->format){
			case GL_RGBA8_OES:	pixelsize = 4;	format = GARNET_PDC_DATA_FORMAT_R8G8B8A8;	break;
			case GL_RGB8_OES:	pixelsize = 3;	format = GARNET_PDC_DATA_FORMAT_R8G8B8;		break;
			case GL_RGB565:		pixelsize = 2;	format = GARNET_PDC_DATA_FORMAT_R5G6B5;		break;
			case GL_RGB5_A1:	pixelsize = 2;	format = GARNET_PDC_DATA_FORMAT_R5G5B5A1;	break;
			case GL_RGBA4:		pixelsize = 2;	format = GARNET_PDC_DATA_FORMAT_R4G4B4A4;	break;
			default: pixelsize = format = 0; break;
		}
		if (dblist->area == NN_GX_MEM_FCRAM)
			burstlen = 0x100;
		else
			burstlen = 0x300;

		addr = (unsigned)dblist->address + pixelsize * (dblist->width * 
			(dblist->height - __sysman.display_height[i] - __sysman.display_yoffset[i]) + __sysman.display_xoffset[i]);
		BASE_GL_FAIL_IF_CONT(addr & 0xf, GL_ERROR_8053_DMP);

		size = pixelsize * dblist->width;

		mode = burstlen | format | (i ? 0 : (1 << 6));

		if (i == 0){
			switch (__sysman.extension_mode){
				case NN_GX_DISPLAYMODE_NORMAL:
					break;
				case NN_GX_DISPLAYMODE_RESERVED:
					mode = burstlen | format;
					break;
				case NN_GX_DISPLAYMODE_STEREO:
					mode = burstlen | format | (2 << 4);
                    
                    {
						db_list_t* dblistB = __sysman.current_buffer[DISP0_EXT_BINDING_POINT];
						BASE_GL_FAIL_IF_CONT(dblistB == 0 || dblistB->address == 0, GL_ERROR_9000_DMP);
						BASE_GL_FAIL_IF_CONT(dblistB->height <= __sysman.display_yoffset[DISP0_EXT_BINDING_POINT]
						 || dblistB->width <= __sysman.display_xoffset[DISP0_EXT_BINDING_POINT], GL_ERROR_9001_DMP);
						BASE_GL_FAIL_IF_CONT(dblist->format != dblistB->format || dblist->width != dblistB->width || dblist->area != dblistB->area, GL_ERROR_9002_DMP);
						addrB = (unsigned)dblistB->address + pixelsize * (dblistB->width * 
							(dblistB->height - __sysman.display_height[i] - __sysman.display_yoffset[DISP0_EXT_BINDING_POINT]) + __sysman.display_xoffset[DISP0_EXT_BINDING_POINT]);
					}
					break;
			}
		}

		nngxlowSetBufferSwap(i, __sysman.next_bank[i], (void*)addr, (void*)addrB, size, mode, __sysman.next_bank[i]);
		__sysman.next_bank[i] = !__sysman.next_bank[i];
	}
	
	return;
}

void nngxGetDisplaybufferParameteri(GLenum pname, GLint* param){
	db_list_t* dblist = __sysman.current_buffer[__sysman.active_display];
	
	if (dblist == 0)
		return;
	
	switch (pname){
		case NN_GX_DISPLAYBUFFER_ADDRESS:
			param[0] = (GLint)dblist->address;
			break;
		case NN_GX_DISPLAYBUFFER_FORMAT:
			param[0] = dblist->format;
			break;
		case NN_GX_DISPLAYBUFFER_WIDTH:
			param[0] = dblist->width;
			break;
		case NN_GX_DISPLAYBUFFER_HEIGHT:
			param[0] = dblist->height;
			break;
		default:
			__err_setError(GL_ERROR_8033_DMP);
			break;
	}
	
	return;
}

void nngxSetDisplayMode(GLenum mode){
	if (__sysman.extension_mode == mode)
		return;
	
	switch (mode){
		case NN_GX_DISPLAYMODE_NORMAL:
			__sysman.display_height[0] = DISP0_HEIGHT;
			break;
		case NN_GX_DISPLAYMODE_RESERVED:
			__sysman.display_height[0] = DISP0_HEIGHT * 2;
			break;
		case NN_GX_DISPLAYMODE_STEREO:
			__sysman.display_height[0] = DISP0_HEIGHT;
			break;
		default:
			__err_setError(GL_ERROR_9003_DMP);
			return;
	}
	__sysman.extension_mode = mode;
	
	return;
}

void nngxStartCmdlistSave(void){
	BASE_GL_FAIL_IF(__sysman.saving_cmdlist, GL_ERROR_8034_DMP);
	BASE_GL_FAIL_IF(!__sysman.bound_cmdlist, GL_ERROR_8035_DMP);
	
	__sysman.bound_cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(__sysman.bound_cmdlist->command_buffer);
	if (__sysman.bound_cmdlist->used_bufsize & 8)
		__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
	
	__sysman.saving_cmdlist = GL_TRUE;
	__sysman.save_cmdbuf_offset = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(__sysman.bound_cmdlist->command_buffer);
	__sysman.save_cmdreq_id = __sysman.bound_cmdlist->used_reqcount;
	
	return;
}

void nngxStopCmdlistSave(GLuint* bufferoffset, GLsizei* buffersize, GLuint* requestid, GLsizei* requestsize){
	BASE_GL_FAIL_IF(!__sysman.saving_cmdlist, GL_ERROR_8036_DMP);
	
	__sysman.saving_cmdlist = GL_FALSE;
	
	__sysman.bound_cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(__sysman.bound_cmdlist->command_buffer);
	if (__sysman.bound_cmdlist->used_bufsize & 8)
		__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
	
	*bufferoffset = __sysman.save_cmdbuf_offset;
	*buffersize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(__sysman.bound_cmdlist->command_buffer) - __sysman.save_cmdbuf_offset;
	*requestid = __sysman.save_cmdreq_id;
	*requestsize = __sysman.bound_cmdlist->used_reqcount - __sysman.save_cmdreq_id;
	
	return;
}

void nngxUseSavedCmdlist(GLuint cmdlist, GLuint bufferoffset, GLsizei buffersize, GLuint requestid, GLsizei requestsize, GLbitfield statemask, GLboolean copycmd){
	int i;
	cl_cmdreq_t* cmdreq = 0;
	cl_list_t* curr_cmdlist = __sysman.bound_cmdlist;
	cl_list_t* saved_cmdlist;
	GLboolean firstren = GL_TRUE;
	GET_CURRENT_STATE(state);
	
	__GET_CL_LIST(cmdlist, saved_cmdlist)
	BASE_GL_FAIL_IF(!curr_cmdlist, GL_ERROR_8037_DMP);
	BASE_GL_FAIL_IF(!saved_cmdlist, GL_ERROR_8038_DMP);
	
	curr_cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(curr_cmdlist->command_buffer);
	
	{
		GLsizei splitsize = 0;
		if (curr_cmdlist->last_split_offset != curr_cmdlist->used_bufsize){
			if (!copycmd)
				splitsize = (curr_cmdlist->used_bufsize & 8) ? 24 : 32;
			else if (requestsize){
				if (saved_cmdlist->command_request[requestid].id != CL_CMDREQ_ID_REN)
					splitsize = (curr_cmdlist->used_bufsize & 8) ? 24 : 32;
				else if (curr_cmdlist->used_bufsize & 8){
					__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
					curr_cmdlist->used_bufsize += 8;
				}
			}
		}
		BASE_GL_FAIL_IF((curr_cmdlist->used_reqcount + requestsize + (splitsize ? 1 : 0) > curr_cmdlist->max_reqcount) ||
			(!copycmd && (curr_cmdlist->used_bufsize + splitsize > curr_cmdlist->max_bufsize)) ||
			(copycmd && (curr_cmdlist->used_bufsize + buffersize + splitsize > curr_cmdlist->max_bufsize)), GL_ERROR_803A_DMP);
		if (splitsize)
			nngxSplitDrawCmdlist();
	}
	
	statemask &= ~state->suppressStateMask;

	if (statemask & NN_GX_STATE_SHADERBINARY)
		state->upd_mask.mask_[0] |= (1 << IF_GL_BINARY_PACKAGE);
	
	if (statemask & NN_GX_STATE_SHADERPROGRAM)
		state->upd_mask.mask_[0] |= (1 << IF_GL_SHADER_UNIFORM);
	
	if (statemask & NN_GX_STATE_SHADERMODE)
		state->upd_mask.mask_[0] |= ((1 << IF_GL_ATTACH_GS) | (1 << IF_GL_DETACH_GS));
	
	if (statemask & NN_GX_STATE_SHADERFLOAT)
		state->upd_mask.mask_[0] |= ((1 << IF_GL_BINARY_EXE_VS) | (1 << IF_GL_BINARY_EXE_GS));
	
	if (statemask & NN_GX_STATE_VSUNIFORM)
		state->upd_mask.mask_[0] |= (1 << IF_GL_SHADER_UNIFORM);
	
	if (statemask & NN_GX_STATE_FSUNIFORM)
		state->upd_mask.mask_[0] |= (1 << IF_HW_RASTERIZER_REGISTER);
	
	if (statemask & NN_GX_STATE_LUT){
		state->upd_mask.mask_[0] |= ((1 << IF_GL_TEXTURE_LUT) | (1 << IF_GL_LIGHTING_LUT_SAMPLERS) |
			(1 << IF_GL_PROCTEX_LUT_SAMPLERS) | (1 << IF_GL_FOG_LUT_SAMPLER) | (1 << IF_GL_GAS_LUT_SAMPLERS));
		for (i = 0; i < LAP_LUT_COUNT; i++){
			state->gl_state.lutbindings.bindings[i] = 0;
			state->gl_state.lutbindings.subsize[i] = 0;
		}
	}
	if (statemask & NN_GX_STATE_TEXTURE)
		state->upd_mask.mask_[0] |= ((1 << IF_GL_TEXTURE0) | (1 << IF_GL_TEXTURE1) | (1 << IF_GL_TEXTURE2));
	
	if (statemask & NN_GX_STATE_FRAMEBUFFER)
		state->upd_mask.mask_[0] |= (1 << IF_GL_FRAMEBUFFER);
	
	if (statemask & NN_GX_STATE_VERTEX)
		state->upd_mask.mask_[0] |= ((1 << IF_GL_VERTBUFFER) | (1 << IF_GL_VERT_ARRAY) | (1 << IF_GL_VERT_CURRENT) | (1 << IF_GL_PROGRAM));
	
	if (statemask & NN_GX_STATE_TRIOFFSET)
		state->upd_mask.mask_[0] |= (1 << IF_GL_TRI_OFFSET);
	
	if (statemask & NN_GX_STATE_FBACCESS)
		state->upd_mask.mask_[0] |= (1 << IF_GL_FB_ACCESS);
	
	if (statemask & NN_GX_STATE_SCISSOR)
		state->upd_mask.mask_[0] |= (1 << IF_GL_FB_SCISSOR);
	
	state->forceValidateMask |= statemask;

	if (requestsize){
		memcpy(&curr_cmdlist->command_request[curr_cmdlist->used_reqcount], &saved_cmdlist->command_request[requestid], sizeof(cl_cmdreq_t) * requestsize);
		cmdreq = &curr_cmdlist->command_request[curr_cmdlist->used_reqcount];
	}
	
	if (copycmd){
		GLsizei copiedbufsize = 0;
		unsigned src_addr, src_size;
		unsigned prev_addr = 0, prev_size = 0;
		
		for (i = 0; i < requestsize; i++){
			if (cmdreq->id == CL_CMDREQ_ID_REN){
				if (firstren){
					firstren = GL_FALSE;
					if (cmdreq->param.ren.extbuf == 0){
						cmdreq->param.ren.size -= bufferoffset + (unsigned)saved_cmdlist->command_buffer - cmdreq->param.ren.addr;
						cmdreq->param.ren.addr = bufferoffset + (unsigned)saved_cmdlist->command_buffer;
					}

					src_addr = cmdreq->param.ren.addr;
					src_size = cmdreq->param.ren.size;

					cmdreq->param.ren.size += curr_cmdlist->used_bufsize - curr_cmdlist->last_split_offset;
					cmdreq->param.ren.addr = (unsigned)curr_cmdlist->command_buffer + curr_cmdlist->last_split_offset;
				}
				else{
					src_addr = cmdreq->param.ren.addr;
					src_size = cmdreq->param.ren.size;
					cmdreq->param.ren.addr = prev_addr + prev_size;
				}

				memcpy(__cb_current_command_buffer, (void*)((unsigned)_PICA_MEM_NATIVE(src_addr)), src_size);
				__cb_current_command_buffer += src_size >> 2;
				if (cmdreq->param.ren.extbuf == 0)
					copiedbufsize += src_size;
				
				cmdreq->param.ren.extbuf = 0;
				prev_addr = cmdreq->param.ren.addr;
				prev_size = cmdreq->param.ren.size;
				
				curr_cmdlist->last_split_offset += cmdreq->param.ren.size;
				
				if (curr_cmdlist->subr_addr)
					sys_fixSubroutineCommand(curr_cmdlist, cmdreq);
			}
			cmdreq->callback_enabled = GL_FALSE;
			cmdreq->stop_enabled = GL_FALSE;
			cmdreq++;
		}
		if (buffersize > copiedbufsize){
			memcpy(__cb_current_command_buffer, (void*)(bufferoffset + (unsigned)_PICA_MEM_NATIVE(saved_cmdlist->command_buffer) + copiedbufsize), buffersize - copiedbufsize);
			__cb_current_command_buffer += (buffersize - copiedbufsize) >> 2;
		}
		curr_cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(curr_cmdlist->command_buffer);
	}
	else{
		for (i = 0; i < requestsize; i++){
			if (cmdreq->id == CL_CMDREQ_ID_REN && cmdreq->param.ren.extbuf == 0){
				if (firstren){

					firstren = GL_FALSE;
					cmdreq->param.ren.size -= bufferoffset + (unsigned)saved_cmdlist->command_buffer - cmdreq->param.ren.addr;
					cmdreq->param.ren.addr = bufferoffset + (unsigned)saved_cmdlist->command_buffer;
				}
				cmdreq->param.ren.extbuf = 1;
			}
			cmdreq->callback_enabled = GL_FALSE;
			cmdreq->stop_enabled = GL_FALSE;
			cmdreq++;
		}
	}
	
	if (requestsize){
		nngxlowLock();
		curr_cmdlist->used_reqcount += requestsize;
		if (curr_cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running){
			__sysman.is_running = GL_TRUE;
			sys_issueSpeculativeCommandRequests();
		}
		nngxlowUnlock();
	}
	
	return;
}

GLsizei nngxExportCmdlist(GLuint cmdlist, GLuint bufferoffset, GLsizei buffersize, GLuint requestid, GLsizei requestsize, GLsizei datasize, GLvoid* data){
	cl_list_t* cmdlist_;
	GLsizei exportedsize = sizeof(cl_exported_header_t) + buffersize + sizeof(cl_cmdreq_t) * requestsize;
	
	__GET_CL_LIST(cmdlist, cmdlist_)
	
	BASE_GL_FAIL_IF_RET(!cmdlist, GL_ERROR_803B_DMP, exportedsize);
	BASE_GL_FAIL_IF_RET(data != 0 && datasize < exportedsize, GL_ERROR_803C_DMP, exportedsize);
	
	if (cmdlist_ == __sysman.bound_cmdlist)
		cmdlist_->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(cmdlist_->command_buffer);
	
	sys_ExportCmdlistCore(cmdlist_, bufferoffset, buffersize, requestid, requestsize, data);
	
	return exportedsize;
}

void __sys_setB2BCommandRequest(void* srcaddr, void* dstaddr, int width, int height, int format){
	cl_cmdreq_t* cmdreq;
	cl_list_t* cmdlist = __sysman.bound_cmdlist;

	BASE_GL_FAIL_IF(cmdlist == 0 || cmdlist->used_reqcount == cmdlist->max_reqcount, GL_ERROR_COMMANDREQUEST_FULL_DMP);

	cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
	cmdreq->id = CL_CMDREQ_ID_PF;
	cmdreq->param.pf.sformat = cmdreq->param.pf.dformat = format;
	cmdreq->param.pf.srcaddr = (unsigned)srcaddr;
	cmdreq->param.pf.dstaddr = (unsigned)dstaddr;
	cmdreq->param.pf.swidth = cmdreq->param.pf.dwidth = width;
	cmdreq->param.pf.sheight = cmdreq->param.pf.dheight = height;
	cmdreq->param.pf.yflip = 0;
	cmdreq->param.pf.amode = GARNET_PPF_AMODE_2x2;
	cmdreq->param.pf.blk32 = 0;
	cmdreq->param.pf.l2b = 0;
	cmdreq->param.pf.b2b = 1;

	nngxlowLock();
	cmdlist->used_reqcount++;
	if (cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running){
		__sysman.is_running = GL_TRUE;
		sys_issueSpeculativeCommandRequests();
	}
	nngxlowUnlock();
	
	return;
}

void sys_ExportCmdlistCore(cl_list_t* cmdlist, GLuint bufferoffset, GLsizei buffersize, GLuint requestid, GLsizei requestsize, GLvoid* data){
	BASE_GL_FAIL_IF(cmdlist->used_bufsize < ((GLsizei)bufferoffset + buffersize) ||
				cmdlist->used_reqcount < (GLsizei)requestid + requestsize, GL_ERROR_803D_DMP);
	BASE_GL_FAIL_IF((bufferoffset & 7) || (buffersize & 7), GL_ERROR_803E_DMP);

	{
		int i;
		GLboolean firstren = GL_TRUE;
		for (i = 0; i < requestsize; i++){
			if (cmdlist->command_request[requestid + i].id == CL_CMDREQ_ID_REN){
				BASE_GL_FAIL_IF(cmdlist->command_request[requestid + i].param.ren.extbuf, GL_ERROR_803F_DMP);
				BASE_GL_FAIL_IF(firstren &&
					((unsigned)cmdlist->command_buffer + bufferoffset < cmdlist->command_request[requestid + i].param.ren.addr ||
					(unsigned)cmdlist->command_buffer + bufferoffset >
						cmdlist->command_request[requestid + i].param.ren.addr + cmdlist->command_request[requestid + i].param.ren.size), GL_ERROR_8040_DMP);
				BASE_GL_FAIL_IF((unsigned)cmdlist->command_buffer + bufferoffset + buffersize <
						cmdlist->command_request[requestid + i].param.ren.addr + cmdlist->command_request[requestid + i].param.ren.size, GL_ERROR_8040_DMP);
				if (firstren)
					firstren = GL_FALSE;
			}
		}
	}
	
	if (data){
		cl_cmdreq_t* cmdreq;
		int i;
		GLboolean firstren = GL_TRUE;

		((cl_exported_header_t*)data)->signature = CL_EXPORT_BIN_SIGNATURE;
		((cl_exported_header_t*)data)->version = CL_EXPORT_BIN_VERSION;
		((cl_exported_header_t*)data)->buffersize = buffersize;
		((cl_exported_header_t*)data)->requestsize = requestsize;

		memcpy(&(((unsigned char*)data)[sizeof(cl_exported_header_t)]), (void*)((unsigned)_PICA_MEM_NATIVE(cmdlist->command_buffer) + bufferoffset), buffersize);

		memcpy(&(((unsigned char*)data)[sizeof(cl_exported_header_t) + buffersize]), &cmdlist->command_request[requestid], sizeof(cl_cmdreq_t) * requestsize);

		cmdreq = (cl_cmdreq_t*)&(((unsigned char*)data)[sizeof(cl_exported_header_t) + buffersize]);
		for (i = 0; i < requestsize; i++){
			if (cmdreq->id == CL_CMDREQ_ID_REN){
				if (firstren){
					firstren = GL_FALSE;
					cmdreq->param.ren.size -= bufferoffset + (unsigned)cmdlist->command_buffer - cmdreq->param.ren.addr;
					cmdreq->param.ren.addr = 0;
				}
				else
					cmdreq->param.ren.addr -= bufferoffset + (unsigned)cmdlist->command_buffer;
			}
			cmdreq->callback_enabled = GL_FALSE;
			cmdreq->stop_enabled = GL_FALSE;
			cmdreq++;
		}
	}
	
	return;
	
}

void nngxUpdateState(GLbitfield statemask){
	GET_CURRENT_STATE(state);
	
	statemask &= ~state->suppressStateMask;
	
	if (statemask & NN_GX_STATE_SHADERBINARY)
		state->upd_mask.mask_[0] |= (1 << IF_GL_BINARY_PACKAGE);
	
	if (statemask & NN_GX_STATE_SHADERPROGRAM)
		state->upd_mask.mask_[0] |= (1 << IF_GL_SHADER_UNIFORM);
	
	if (statemask & NN_GX_STATE_SHADERMODE)
		state->upd_mask.mask_[0] |= ((1 << IF_GL_ATTACH_GS) | (1 << IF_GL_DETACH_GS));
	
	if (statemask & NN_GX_STATE_SHADERFLOAT)
		state->upd_mask.mask_[0] |= ((1 << IF_GL_BINARY_EXE_VS) | (1 << IF_GL_BINARY_EXE_GS));
	
	if (statemask & NN_GX_STATE_VSUNIFORM)
		state->upd_mask.mask_[0] |= (1 << IF_GL_SHADER_UNIFORM);
	
	if (statemask & NN_GX_STATE_FSUNIFORM)
		state->upd_mask.mask_[0] |= (1 << IF_HW_RASTERIZER_REGISTER);
	
	if (statemask & NN_GX_STATE_LUT){
		int i;
		state->upd_mask.mask_[0] |= ((1 << IF_GL_TEXTURE_LUT) | (1 << IF_GL_LIGHTING_LUT_SAMPLERS) |
			(1 << IF_GL_PROCTEX_LUT_SAMPLERS) | (1 << IF_GL_FOG_LUT_SAMPLER) | (1 << IF_GL_GAS_LUT_SAMPLERS));
		for (i = 0; i < LAP_LUT_COUNT; i++){
			state->gl_state.lutbindings.bindings[i] = 0;
			state->gl_state.lutbindings.subsize[i] = 0;
		}
	}
	if (statemask & NN_GX_STATE_TEXTURE)
		state->upd_mask.mask_[0] |= ((1 << IF_GL_TEXTURE0) | (1 << IF_GL_TEXTURE1) | (1 << IF_GL_TEXTURE2));
	
	if (statemask & NN_GX_STATE_FRAMEBUFFER)
		state->upd_mask.mask_[0] |= (1 << IF_GL_FRAMEBUFFER);
	
	if (statemask & NN_GX_STATE_VERTEX)
		state->upd_mask.mask_[0] |= ((1 << IF_GL_VERTBUFFER) | (1 << IF_GL_VERT_ARRAY) | (1 << IF_GL_VERT_CURRENT) | (1 << IF_GL_PROGRAM));
	
	if (statemask & NN_GX_STATE_TRIOFFSET)
		state->upd_mask.mask_[0] |= (1 << IF_GL_TRI_OFFSET);
	
	if (statemask & NN_GX_STATE_FBACCESS)
		state->upd_mask.mask_[0] |= (1 << IF_GL_FB_ACCESS);
	
	if (statemask & NN_GX_STATE_SCISSOR)
		state->upd_mask.mask_[0] |= (1 << IF_GL_FB_SCISSOR);
	
	state->forceValidateMask |= statemask;
	
	return;
}

void nngxAdd3DCommand(const GLvoid* bufferaddr, GLsizei buffersize, GLboolean copycmd){
	cl_list_t* cmdlist = __sysman.bound_cmdlist;
	
	BASE_GL_FAIL_IF(!cmdlist, GL_ERROR_804E_DMP);
	
	BASE_GL_FAIL_IF(buffersize <= 0, GL_ERROR_804F_DMP);
	cmdlist->used_bufsize = (unsigned)__cb_current_command_buffer - (unsigned)_PICA_MEM_NATIVE(cmdlist->command_buffer);
	
	if (copycmd){
		BASE_GL_FAIL_IF(buffersize & 3, GL_ERROR_804F_DMP);
		BASE_GL_FAIL_IF(cmdlist->used_bufsize + buffersize > cmdlist->max_bufsize, GL_ERROR_8050_DMP);
		memcpy(__cb_current_command_buffer, (GLuint*)bufferaddr, buffersize);
		__cb_current_command_buffer += buffersize >> 2;
	}

	else{
		BASE_GL_FAIL_IF(buffersize & 0xf, GL_ERROR_804F_DMP);
		BASE_GL_FAIL_IF((unsigned)bufferaddr & 0xf, GL_ERROR_8052_DMP);
		BASE_GL_FAIL_IF(cmdlist->used_reqcount + (cmdlist->last_split_offset != cmdlist->used_bufsize ? 2 : 1) > cmdlist->max_reqcount, GL_ERROR_8051_DMP);
		if (cmdlist->last_split_offset != cmdlist->used_bufsize)
			nngxSplitDrawCmdlist();
		if (cmdlist->last_split_offset == cmdlist->used_bufsize){
			cl_cmdreq_t* cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
			cmdreq->id = CL_CMDREQ_ID_REN;
			cmdreq->param.ren.addr = (unsigned)bufferaddr;
			cmdreq->param.ren.size = buffersize;
			cmdreq->param.ren.extbuf = 1;
			nngxlowLock();
			cmdlist->used_reqcount++;
			if (cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running){
				__sysman.is_running = GL_TRUE;
				sys_issueSpeculativeCommandRequests();
			}
			nngxlowUnlock();
		}
	}
	
	return;
}

void nngxAddVramDmaCommandRaw(nngxCommandList* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei size){
	sys_AddVramDmaCommandCore((cl_list_t*)cmdlist, srcaddr, dstaddr, size);
}

void sys_AddVramDmaCommandCore(cl_list_t* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei size){
	cl_cmdreq_t* cmdreq;

	BASE_GL_FAIL_IF(cmdlist == 0 || cmdlist->used_reqcount == cmdlist->max_reqcount, GL_ERROR_8062_DMP);
	BASE_GL_FAIL_IF(size < 0, GL_ERROR_8064_DMP);

	cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
	
	cmdreq->id = CL_CMDREQ_ID_DMA;
	cmdreq->param.dma.srcaddr = (unsigned)srcaddr;
	cmdreq->param.dma.dstaddr = (unsigned)dstaddr;
	cmdreq->param.dma.size = size;

	nngxlowLock();
	cmdlist->used_reqcount++;
	if (cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running){
		__sysman.is_running = GL_TRUE;
		sys_issueSpeculativeCommandRequests();
	}
	nngxlowUnlock();
	
	return;
}

void nngxAddMemoryFillCommand(GLvoid* startaddr0, GLsizei size0, GLuint data0, GLsizei width0, GLvoid* startaddr1, GLsizei size1, GLuint data1, GLsizei width1){
	sys_AddMemoryFillCommandCore(__sysman.bound_cmdlist, startaddr0, size0, data0, width0, startaddr1, size1, data1, width1);
}

void sys_AddMemoryFillCommandCore(cl_list_t* cmdlist, GLvoid* startaddr0, GLsizei size0, GLuint data0, GLsizei width0, GLvoid* startaddr1, GLsizei size1, GLuint data1, GLsizei width1){
	cl_cmdreq_t* cmdreq;
	
	BASE_GL_FAIL_IF(cmdlist == 0 || cmdlist->used_reqcount == cmdlist->max_reqcount, GL_ERROR_8078_DMP);
	BASE_GL_FAIL_IF(((unsigned)startaddr0 & 0xf) || ((unsigned)startaddr1 & 0xf), GL_ERROR_8079_DMP);
	BASE_GL_FAIL_IF(((startaddr0 != 0) && (size0 & 0xf)) || ((startaddr1 != 0) && (size1 & 0xf)), GL_ERROR_807A_DMP);
	BASE_GL_FAIL_IF((startaddr0 != 0) && (width0 != 16) && (width0 != 24) && (width0 != 32), GL_ERROR_807B_DMP);
	BASE_GL_FAIL_IF((startaddr1 != 0) && (width1 != 16) && (width1 != 24) && (width1 != 32), GL_ERROR_807B_DMP);

	cmdreq = &cmdlist->command_request[cmdlist->used_reqcount];
	
	cmdreq->id = CL_CMDREQ_ID_MF;
	cmdreq->param.mf.start0 = (unsigned)startaddr0;
	cmdreq->param.mf.data0 = data0;
	cmdreq->param.mf.size0 = size0;
	switch (width0){
		case 16: cmdreq->param.mf.format0 = GARNET_PSC_FILL_FORMAT_16; break;
		case 24: cmdreq->param.mf.format0 = GARNET_PSC_FILL_FORMAT_24; break;
		case 32: cmdreq->param.mf.format0 = GARNET_PSC_FILL_FORMAT_32; break;
	}
	cmdreq->param.mf.start1 = (unsigned)startaddr1;
	cmdreq->param.mf.data1 = data1;
	cmdreq->param.mf.size1 = size1;
	switch (width1){
		case 16: cmdreq->param.mf.format1 = GARNET_PSC_FILL_FORMAT_16; break;
		case 24: cmdreq->param.mf.format1 = GARNET_PSC_FILL_FORMAT_24; break;
		case 32: cmdreq->param.mf.format1 = GARNET_PSC_FILL_FORMAT_32; break;
	}

	nngxlowLock();
	cmdlist->used_reqcount++;
	if (cmdlist == __sysman.run_cmdlist && __sysman.start_run && !__sysman.is_running){
		__sysman.is_running = GL_TRUE;
		sys_issueSpeculativeCommandRequests();
	}
	nngxlowUnlock();
	
	return;
}

static void sys_fixSubroutineCommand(cl_list_t* cmdlist, cl_cmdreq_t* cmdreq){
	cmdlist->subr_addr[SUBROUTINE_COMMAND_CH2_SIZE] = ((unsigned)_PICA_MEM_NATIVE(cmdreq->param.ren.addr + cmdreq->param.ren.size)
														- (unsigned)(cmdlist->subr_addr + SUBROUTINE_COMMAND_NUM)) >> 3;

	nngxlowFlushDataCache((void*)cmdreq->param.ren.addr, cmdreq->param.ren.size);
	cmdreq->param.ren.noflush = 1;

	cmdreq->param.ren.size = cmdlist->subr_bufsize;

	cmdlist->subr_addr = 0;
	cmdlist->subr_bufsize = 0;
}