/* 
+----------------------------------------------------------------------+
| (c) Copyright IBM Corporation 2005.                                  | 
| All Rights Reserved.                                                 |
+----------------------------------------------------------------------+ 
|                                                                      | 
| Licensed under the Apache License, Version 2.0 (the "License"); you  | 
| may not use this file except in compliance with the License. You may | 
| obtain a copy of the License at                                      | 
|  http://www.apache.org/licenses/LICENSE-2.0                          |
|                                                                      | 
| Unless required by applicable law or agreed to in writing, software  | 
| distributed under the License is distributed on an "AS IS" BASIS,    | 
| WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      | 
| implied. See the License for the specific language governing         | 
| permissions and limitations under the License.                       | 
+----------------------------------------------------------------------+ 
| Author: Pete Robbins                                                 | 
+----------------------------------------------------------------------+ 

*/
/* $Id$ */

#ifndef _XMLDASEXPORT_H_
#define _XMLDASEXPORT_H_
#define XMLDAS4CPP_VERSION 20050715

#if defined(WIN32)  || defined (_WINDOWS)
#ifdef XMLDAS_EXPORTS
#pragma warning(disable: 4786)
#define XMLDAS_API __declspec(dllexport)
#else
#define XMLDAS_API __declspec(dllimport)
#endif
#elif defined( __linux__ )
#define XMLDAS_API
#endif

#endif // _XMLDASEXPORT_H_

