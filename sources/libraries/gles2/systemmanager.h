#pragma once

#include "common.h"

enum{
	CL_CMDREQ_ID_DMA,
	CL_CMDREQ_ID_REN,
	CL_CMDREQ_ID_MF,
	CL_CMDREQ_ID_PF,
	CL_CMDREQ_ID_CTX
};

typedef struct CL_CMDREQ_T{
	unsigned char			id;
	GLboolean				callback_enabled;
	GLboolean				stop_enabled;
	char                    padding1;
	
	union cl_cmdreq_param_t{
		struct cl_cmdreq_contentd_t{
			unsigned	d[6];
		} data;
		struct cl_cmdreq_dma_t{
			unsigned	srcaddr		: 32 ;
			unsigned	dstaddr		: 32 ;
			unsigned	size		: 32 ;
			unsigned	noflush		: 1  ;
			unsigned	rsv0		: 31 ;
			unsigned	rsv1		: 32 ;
			unsigned	rsv2		: 32 ;
		} dma;
		struct cl_cmdreq_ren_t{
			unsigned	addr		: 32 ;
			unsigned	size		: 32 ;
			unsigned	gas			: 1  ;
			unsigned	extbuf		: 1  ;
			unsigned	noflush		: 1  ;
			unsigned	rsv0		: 29 ;
			unsigned	rsv1		: 32 ;
			unsigned	rsv2		: 32 ;
			unsigned	rsv3		: 32 ;
		} ren;
		struct cl_cmdreq_mf_t{
			unsigned	start0		: 32 ;
			unsigned	data0		: 32 ;
			unsigned	size0		: 23 ;
			unsigned	format0		: 2  ;
			unsigned	rsv0		: 7  ;
			unsigned	start1		: 32 ;
			unsigned	data1		: 32 ;
			unsigned	size1		: 23 ;
			unsigned	format1		: 2  ;
			unsigned	rsv1		: 7  ;
		} mf;
		struct cl_cmdreq_pf_t{
			unsigned	srcaddr		: 32 ;
			unsigned	dstaddr		: 32 ;
			unsigned	swidth		: 16 ;
			unsigned	sheight		: 16 ;
			unsigned	dwidth		: 16 ;
			unsigned	dheight		: 16 ;
			unsigned	sformat		: 3  ;
			unsigned	dformat		: 3  ;
			unsigned	yflip		: 1  ;
			unsigned	amode		: 2  ;
			unsigned	blk32		: 1  ;
			unsigned	l2b			: 1  ;
			unsigned	b2b			: 1  ;
			unsigned	rsv0		: 20 ;
			unsigned	rsv1		: 32 ;
		} pf;
		struct cl_cmdreq_ctx_t{
			unsigned	srcaddr		: 32 ;
			unsigned	dstaddr		: 32 ;
			unsigned	size		: 32 ;
			unsigned	sintiv		: 16 ;
			unsigned	sintv		: 16 ;
			unsigned	dintiv		: 16 ;
			unsigned	dintv		: 16 ;
			unsigned	rsv0		: 32 ;
		} ctx;
	} param;
} cl_cmdreq_t;


typedef struct CL_LIST_T
{
	GLuint					id;
	unsigned*				command_buffer;
	GLsizei					max_bufsize;
	GLsizei					used_bufsize;
	GLsizei					run_bufsize;
	GLsizei					last_split_offset;
	cl_cmdreq_t*			command_request;
	GLsizei					max_reqcount;
	GLsizei					used_reqcount;
	GLsizei					issued_reqcount;
	GLsizei					run_reqcount;
	GLenum					run_mode;
	GLsizei					subr_bufsize;
	unsigned*				subr_addr;
	void					(*callbackfunc)(GLint);
	GLboolean				last_callback_enabled;
	GLboolean				gas_accumulation;
	char                    padding2[2];
	struct CL_LIST_T*		next;
} cl_list_t;

typedef struct CL_EXPORTED_HEADER_T{
	unsigned				signature;
	unsigned				version;
	unsigned				buffersize;
	unsigned				requestsize;
} cl_exported_header_t;

#define CL_EXPORT_BIN_SIGNATURE	0x42454344
#define CL_EXPORT_BIN_VERSION	0x00010000

#define CL_LIST_TABLE_SIZE		0x1f
#define DB_LIST_TABLE_SIZE		0x1f

typedef struct DB_LIST_T{
	GLuint					id;
	void*					address;
	GLenum					format;
	GLsizei					width;
	GLsizei					height;
	GLenum					area;
	struct DB_LIST_T*		next;
} db_list_t;

typedef struct SYS_MANAGER_T{
	GLsizei					display_width[2];
	GLsizei					display_height[2];
	GLboolean				initialized;
	GLboolean				is_running;
	GLboolean				start_run;
	GLboolean				saving_cmdlist;
	int						num_speculative;
	GLboolean				stop_speculative;
	char                    padding3[3];
	cl_list_t*				cl_list_table[32];
	cl_list_t*				bound_cmdlist;
	cl_list_t*				run_cmdlist;
	db_list_t*				db_list_table[32];
	int						active_display;
	db_list_t*				current_buffer[3];
	GLint					display_xoffset[3];
	GLint					display_yoffset[3];
	int						next_bank[2];
	GLint					v_counter[2];
	void					(*v0callback)(GLenum);
	void					(*v1callback)(GLenum);
	GLenum					extension_mode;
	unsigned				save_cmdbuf_offset;
	GLuint					save_cmdreq_id;
	void					(*tocallback)(void);
	s32                     padding4;
	s64						timeout;
} sys_manager_t;

#define V_COUNTER_LIMIT		0x7ffffffe

#define PICA_3D_INTR_CODE			0x12345678

#define DISP0_EXT_BINDING_POINT	2

#define DISP0_WIDTH			240
#define DISP0_HEIGHT		400
#define DISP1_WIDTH			240
#define DISP1_HEIGHT		320

void __sys_markGasAcmMax(void);
void __sys_setDMACommandRequest(void* dstaddr, void* srcaddr, unsigned size);
void __sys_setB2BCommandRequest(void* srcaddr, void* dstaddr, int width, int height, int format);