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

// XMLHelperImpl.cpp: implementation of the XMLHelperImpl class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable: 4786)
#include "SDOXMLFileWriter.h"   // Include first to avoid libxml compile problems!
#include "SDOXMLStreamWriter.h" // Include first to avoid libxml compile problems!
#include "SDOXMLBufferWriter.h" // Include first to avoid libxml compile problems!
#include "XMLHelperImpl.h"
#include "XMLDocumentImpl.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "SDOSAX2Parser.h"
#include "XSDPropertyInfo.h"
#include "XSDTypeInfo.h"


namespace commonj
{
	namespace sdo
	{
		
		namespace xmldas
		{
			
			//////////////////////////////////////////////////////////////////////
			// Construction/Destruction
			//////////////////////////////////////////////////////////////////////
			
			XMLHelperImpl::XMLHelperImpl(DataFactoryPtr df)
			{
				dataFactory = (DASDataFactory*)(DataFactory*)df;
			}
			
			XMLHelperImpl::~XMLHelperImpl()
			{
			}

			DASDataFactoryPtr XMLHelperImpl::getDataFactory()
			{
				if (!dataFactory) 
				{
					dataFactory = DASDataFactory::getDataFactory();
				}
				return dataFactory;
			}

			XMLDocumentPtr XMLHelperImpl::createDocument(DataObjectPtr dataObject)
			{
				
				SDOXMLString rootElementName;
				if (dataObject)
				{
					XSDTypeInfo* ti = (XSDTypeInfo*)
						(getDataFactory()->getDASValue(dataObject->getType(), "XMLDAS::TypeInfo"));
					if (ti)
					{
						rootElementName = ti->getTypeDefinition().localname;
					}
				}

				return new XMLDocumentImpl(dataObject, NULL, rootElementName);
			}

			XMLDocumentPtr XMLHelperImpl::createDocument(
				DataObjectPtr dataObject,
				const char* rootElementURI,
				const char* rootElementName)
			{
				return new XMLDocumentImpl(dataObject, rootElementURI, rootElementName);
			}
			
			XMLDocumentPtr XMLHelperImpl::loadFile(
				const char* xmlFile,
				const char* targetNamespaceURI)
			{
				DataObjectPtr rootDataObject;
				SDOSAX2Parser sdoParser(getDataFactory(), targetNamespaceURI, rootDataObject);
				sdoParser.parse(xmlFile);				
				return createDocument(rootDataObject);
			}
			
			XMLDocumentPtr XMLHelperImpl::load(
				istream& inXml,
				const char* targetNamespaceURI)
			{
				DataObjectPtr rootDataObject;
				SDOSAX2Parser sdoParser(getDataFactory(), targetNamespaceURI, rootDataObject);
				inXml>>sdoParser;
				return createDocument(rootDataObject);
			}
			
			XMLDocumentPtr XMLHelperImpl::load(
				const char* inXml,
				const char* targetNamespaceURI)
			{
				istringstream str(inXml);
                return load(str, targetNamespaceURI);
			}
			
			void XMLHelperImpl::save(XMLDocumentPtr doc, const char* xmlFile)
			{
				SDOXMLFileWriter writer(xmlFile, dataFactory);
				writer.write(doc);
			}
			
			void XMLHelperImpl::save(
				DataObjectPtr dataObject,
				const char* rootElementURI,
				const char* rootElementName,
				const char* xmlFile)
			{
				save(createDocument(dataObject,rootElementURI, rootElementName), xmlFile);
			}
			
			
			// Serializes the datagraph to a stream
			void XMLHelperImpl::save(XMLDocumentPtr doc, ostream& outXml)
			{
				SDOXMLStreamWriter writer(outXml, dataFactory);
				writer.write(doc);				
			}
			void XMLHelperImpl::save(
				DataObjectPtr dataObject,
				const char* rootElementURI,
				const char* rootElementName,
				std::ostream& outXml)
			{
				save(createDocument(dataObject,rootElementURI, rootElementName), outXml);
			}
			
			// Serializes the datagraph to a string
			char* XMLHelperImpl::save(XMLDocumentPtr doc)
			{
				SDOXMLBufferWriter writer(dataFactory);
				writer.write(doc);
				SDOXMLString ret = writer.getBuffer();
				char* retString = new char[strlen(ret) +1];
				strcpy(retString, ret);
				return retString;
			}
			char* XMLHelperImpl::save(
				DataObjectPtr dataObject,
				const char* rootElementURI,
				const char* rootElementName)
			{
				return save(createDocument(dataObject,rootElementURI, rootElementName));
			}
			
			
		} // End - namespace xmldas
	} // End - namespace sdo
} // End - namespace commonj
