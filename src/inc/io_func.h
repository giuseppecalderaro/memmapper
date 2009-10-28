/********************************************************************************
 * This file is part of memmapper.						*
 * 										*
 * memmapper is free software: you can redistribute it and/or modify		*
 * it under the terms of the GNU General Public License as published by		*
 * the Free Software Foundation, either version 3 of the License, or		*
 * (at your option) any later version.						*
 * memmapper is distributed in the hope that it will be useful,			*
 * but WITHOUT ANY WARRANTY; without even the implied warranty of		*
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		*
 * GNU General Public License for more details.					*
 * You should have received a copy of the GNU General Public License		*
 * along with memmapper.  If not, see <http://www.gnu.org/licenses/>.		*
 * 										*
 * Author: 	Giuseppe Calderaro						*
 * Email: 	giuseppecalderaro@gmail.com					*
 * 										*
 ********************************************************************************/

#ifndef IO_FUNC_H_
#define IO_FUNC_H_

void io_rw(unsigned short int port, unsigned long *data, int length, int op);

#endif /*IO_FUNC_H_*/
