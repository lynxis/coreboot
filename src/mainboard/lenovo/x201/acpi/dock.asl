/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include "smi.h"

Scope (\_SB)
{
	Device(DOCK)
	{
		Name(_HID, "ACPI0003")
		Name(_UID, 0x00)
		Name(_PCL, Package() { \_SB } )
		Method(_DCK, 1, NotSerialized)
		{
			if (Arg0) {
			   /* connect dock */
			   Store (1, \GP28)
			   Store (1, \_SB.PCI0.LPCB.EC.DKR1)
			   Store (1, \_SB.PCI0.LPCB.EC.DKR2)
			   Store (1, \_SB.PCI0.LPCB.EC.DKR3)
			} else {
			   /* disconnect dock */
			   Store (0, \GP28)
			   Store (0, \_SB.PCI0.LPCB.EC.DKR1)
			   Store (0, \_SB.PCI0.LPCB.EC.DKR2)
			   Store (0, \_SB.PCI0.LPCB.EC.DKR3)
			}
			Return (1) /* Success */
		}

		Method(_STA, 0, NotSerialized)
		{
			Return (\_SB.PCI0.LPCB.EC.DKR1)
		}

		Method(CHCK, 0, NotSerialized) /* return 1 if docked, 0 if undocked */
		{
			if (\_SB.PCI0.LPCB.EC.DKR1) {
				Return (1)
			} else {
				Return (0)
			}
		}
	}
}

Scope(\_SB.PCI0.LPCB.EC)
{
	/* press docking key on the dock -> what event comes in? */
	Method(_Q18, 0, NotSerialized) /* undocking key on keyboard */
	{
		Notify(\_SB.DOCK, 3)
	}

	Method(_Q45, 0, NotSerialized) /* docking is attached or detached */
	{
		Notify(\_SB.DOCK, 0)
	}

	Method(_Q58, 0, NotSerialized) /* ?? */
	{
		Notify(\_SB.DOCK, 0)
	}

	Method(_Q37, 0, NotSerialized) /* dock event */
	{
		Notify(\_SB.DOCK, 0)
	}
}
