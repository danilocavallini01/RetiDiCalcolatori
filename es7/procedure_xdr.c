/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "procedure.h"

bool_t
xdr_FileScanRes (XDR *xdrs, FileScanRes *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->characters))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->words))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->lines))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->errno))
				 return FALSE;
		} else {
			IXDR_PUT_LONG(buf, objp->characters);
			IXDR_PUT_LONG(buf, objp->words);
			IXDR_PUT_LONG(buf, objp->lines);
			IXDR_PUT_LONG(buf, objp->errno);
		}
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->characters))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->words))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->lines))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->errno))
				 return FALSE;
		} else {
			objp->characters = IXDR_GET_LONG(buf);
			objp->words = IXDR_GET_LONG(buf);
			objp->lines = IXDR_GET_LONG(buf);
			objp->errno = IXDR_GET_LONG(buf);
		}
	 return TRUE;
	}

	 if (!xdr_int (xdrs, &objp->characters))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->words))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->lines))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->errno))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_DirInput (XDR *xdrs, DirInput *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->threshold))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->dir, 100))
		 return FALSE;
	return TRUE;
}