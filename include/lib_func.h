/********************************************************************************
 * This file is part of memmapper.												*
 * 										                                        *
 * memmapper is free software: you can redistribute it and/or modify		    *
 * it under the terms of the GNU General Public License as published by		    *
 * the Free Software Foundation, either version 3 of the License, or		    *
 * (at your option) any later version.						                    *
 * memmapper is distributed in the hope that it will be useful,			        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of		        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		        *
 * GNU General Public License for more details.					                *
 * You should have received a copy of the GNU General Public License		    *
 * along with memmapper.  If not, see <http://www.gnu.org/licenses/>.		    *
 * 										                                        *
 * Author: 	Giuseppe Calderaro						                            *
 * Email: 	giuseppecalderaro@gmail.com					                        *
 * 										                                        *
 ********************************************************************************/

#ifndef LIB_FUNC_H_
#define LIB_FUNC_H_

#define str(arg) #arg

#define BYTES_PER_WORD	__WORDSIZE / 8
#define NIBBLES_PER_WORD str(16)

#define KB (1 << 10)
#define MB (1 << 20)
#define GB (1 << 30)

unsigned long hex_encoder(const char *address, int *sign);

#endif /*LIB_FUNC_H_*/
