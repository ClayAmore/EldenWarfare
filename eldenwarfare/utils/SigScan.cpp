#include "SigScan.h"
#include "hde/hde64.h"

namespace EW
{
	intptr_t Signature::Scan() const
	{
		mem::default_scanner scanner(pattern);
		if (mem::pointer ptr = scanner.scan(scan_region)) {
			auto addr = ptr.as<intptr_t>() + offset;
			hde64s instr{};

			switch (type) {
			case Type::Address:
				return addr;
			case Type::InInstruction:
				hde64_disasm((void*)addr, &instr);
				// call, jmp, etc
				if (instr.flags & F_RELATIVE) {
					intptr_t imm = 0;
					if (instr.flags & F_IMM8) imm = instr.imm.imm8;
					else if (instr.flags & F_IMM16) imm = instr.imm.imm16;
					else if (instr.flags & F_IMM32) imm = instr.imm.imm32;
					else imm = instr.imm.imm64;
					
					return addr + instr.len + imm;
				}
				// Access to static memory address
				else if (instr.flags & F_MODRM && instr.modrm_mod == 0 && instr.modrm_rm == 0b101) {
					return addr + instr.len + (intptr_t)instr.disp.disp32;
				}
				else return 0;
			default:
				throw std::runtime_error(std::format("Signature type not implemented: {}", static_cast<uint32_t>(type)));
			}
		}
		else return 0;
	}
}
