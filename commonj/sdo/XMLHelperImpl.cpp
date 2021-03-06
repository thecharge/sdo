/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *   
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/* $Rev: 549872 $ $Date$ */

#include "commonj/sdo/SDOXMLFileWriter.h"   // Include first to avoid libxml compile problems!
#include "commonj/sdo/SDOXMLStreamWriter.h" // Include first to avoid libxml compile problems!
#include "commonj/sdo/SDOXMLBufferWriter.h" // Include first to avoid libxml compile problems!
#include "commonj/sdo/XMLHelperImpl.h"
#include "commonj/sdo/XMLDocumentImpl.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "commonj/sdo/SDOSAX2Parser.h"
#include "commonj/sdo/XSDPropertyInfo.h"
#include "commonj/sdo/XSDTypeInfo.h"
#include "commonj/sdo/SDORuntimeException.h"
#include "commonj/sdo/DataFactoryImpl.h"


namespace commonj
{
    namespace sdo
    {
        
        
        //////////////////////////////////////////////////////////////////////
        // Construction/Destruction
        //////////////////////////////////////////////////////////////////////
        
        XMLHelperImpl::XMLHelperImpl(DataFactoryPtr df)
        {
            dataFactory = (DataFactory*)df;
            if (!dataFactory) 
            {
                dataFactory = DataFactory::getDataFactory();
            }
        }
        
        XMLHelperImpl::~XMLHelperImpl()
        {
            clearErrors();
        }

        DataFactoryPtr XMLHelperImpl::getDataFactory()
        {
            if (!dataFactory) 
            {
                dataFactory = DataFactory::getDataFactory();
            }
            return dataFactory;
        }


      const TypeImpl* XMLHelperImpl::findRoot(DataFactory* df,
                                                  const SDOString& rootElementURI)
      {

        const TypeImpl* rootTypePtr = ((DataFactoryImpl*)df)->findTypeImpl(rootElementURI.c_str(),
                                                                           "RootType");

        if (rootTypePtr != 0)
        {
           return rootTypePtr;
        }

        const TypeList& tl = df->getTypes();
        for (unsigned int i = 0; i < tl.size(); i++)
        {
           if (!strcmp("RootType", tl[i].getName()))
           {
              return ((DataFactoryImpl*)df)->findTypeImpl(tl[i].getURI(), "RootType");
           }
        }
        return 0;
      }

        XMLDocumentPtr XMLHelperImpl::createDocument(const char* elementname,
                                                     const char* rootElementURI)
        {
           if (rootElementURI == 0)
           {
              if (elementname == 0)
              {
                 return createDocument(SDOString(""), SDOString(""));
              }
              else
              {
                 return createDocument(SDOString(elementname), SDOString(""));
              }
           }
           else
           {
              if (elementname == 0)
              {
                 return createDocument(SDOString(""), SDOString(rootElementURI));
              }
              else
              {
                 return createDocument(SDOString(elementname), SDOString(rootElementURI));
              }
           }
        }


       XMLDocumentPtr XMLHelperImpl::createDocument(const SDOString& elementname,
                                                    const SDOString& rootElementURI)
       {
          DataFactory* dp = (DataFactory*)getDataFactory();
          if (dp == 0) return 0;

          const TypeImpl* rType = findRoot(dp, rootElementURI);
          if (rType == 0)
          {
             std::string msg("createDocument - cannot find element ");
             msg += elementname;
             SDO_THROW_EXCEPTION("createDocument", SDOUnsupportedOperationException,
                                 msg.c_str());
          }

          if (elementname.length() != 0)
          {
             PropertyImpl* pl = rType->getPropertyImpl(elementname);
             if (pl == 0)
             {
                std::string msg("createDocument - cannot find element ");
                msg += elementname;
                SDO_THROW_EXCEPTION("createDocument", SDOUnsupportedOperationException,
                                    msg.c_str());
             }

             const Type& tp = pl->getType();
             DataObjectPtr dob = dp->create(tp);
             return new XMLDocumentImpl(dob,
                                        tp.getURI(),  
                                        elementname.c_str());
          }
          else
          {
             const Property& pl = rType->getProperty((unsigned int)0);
             const Type& tp = pl.getType();
             DataObjectPtr dob = dp->create(tp);
             return new XMLDocumentImpl(dob,
                                        tp.getURI(), pl.getName());
          }

          std::string msg("createDocument - unable to find root type in namespace ");
             msg += " NULL";
          SDO_THROW_EXCEPTION("createDocument", SDOUnsupportedOperationException,
                              msg.c_str());
       }

        XMLDocumentPtr XMLHelperImpl::createDocument(
            DataObjectPtr dataObject,
            const char* rootElementURI,
            const char* rootElementName)
        {
        	SDOString uri;
        	SDOString name;
        	if (0 == rootElementURI)
        	  uri = "";
        	  else
        	  uri =  rootElementURI;
        	  if (0 == rootElementName)
        	  name = "";
        	  else
        	  name =  rootElementName;
        	  
            return createDocument(dataObject, uri, name);
        }
        XMLDocumentPtr XMLHelperImpl::createDocument(
            DataObjectPtr dataObject,
            const SDOString& rootElementURI,
            const SDOString& rootElementName)
        {
            return new XMLDocumentImpl(dataObject, rootElementURI.c_str(), rootElementName.c_str());
        }
        
        XMLDocumentPtr XMLHelperImpl::loadFile(
            const char* xmlFile,
            const char* targetNamespaceURI)
        {
            DataObjectPtr rootDataObject;
            clearErrors();
            SDOSAX2Parser sdoParser(getDataFactory(),
                                    targetNamespaceURI,
                                    rootDataObject,
                                    this);
            if (sdoParser.parse(xmlFile) == 0)
            {                
                return createDocument(rootDataObject, (const char*)sdoParser.getRootElementURI(), sdoParser.getRootElementName());
            }
            return 0;
        }

        XMLDocumentPtr XMLHelperImpl::loadFile(
            const SDOString& xmlFile,
            const SDOString& targetNamespaceURI)
        {
            DataObjectPtr rootDataObject;
            clearErrors();
            SDOSAX2Parser sdoParser(getDataFactory(),
                                    targetNamespaceURI.c_str(),
                                    rootDataObject,
                                    this);
            if (sdoParser.parse(xmlFile.c_str()) == 0)
            {                
                return createDocument(rootDataObject, (const char*)sdoParser.getRootElementURI(),   sdoParser.getRootElementName());
            }
            return 0;
        }

        
        XMLDocumentPtr XMLHelperImpl::load(
            std::istream& inXml,
            const char* targetNamespaceURI)
        {
            DataObjectPtr rootDataObject;
            SDOSAX2Parser sdoParser(getDataFactory(),
                                    targetNamespaceURI,
                                    rootDataObject,
                                    this);
            clearErrors();
            inXml>>sdoParser;
            return createDocument(rootDataObject, (const char*)sdoParser.getRootElementURI(),   sdoParser.getRootElementName());
       }

        XMLDocumentPtr XMLHelperImpl::load(
            std::istream& inXml,
            const SDOString& targetNamespaceURI)
        {
            DataObjectPtr rootDataObject;
            SDOSAX2Parser sdoParser(getDataFactory(),
                                    targetNamespaceURI.c_str(),
                                    rootDataObject,
                                    this);
            clearErrors();
            inXml>>sdoParser;
            return createDocument(rootDataObject, (const char*)sdoParser.getRootElementURI(), sdoParser.getRootElementName());
        }
        
        XMLDocumentPtr XMLHelperImpl::load(
            const char* inXml,
            const char* targetNamespaceURI)
        {
            std::istringstream str(inXml);
            return load(str, targetNamespaceURI);
        }
        XMLDocumentPtr XMLHelperImpl::load(
            const SDOString& inXml,
            const SDOString& targetNamespaceURI)
        {
            std::istringstream str(inXml);
            return load(str, targetNamespaceURI);
        }

        void XMLHelperImpl::save(XMLDocumentPtr doc, const char* xmlFile, int indent)
        {
            SDOXMLFileWriter writer(xmlFile, dataFactory);
            writer.write(doc, indent);
        }
        void XMLHelperImpl::save(XMLDocumentPtr doc, const SDOString& xmlFile, int indent)
        {
            SDOXMLFileWriter writer(xmlFile.c_str(), dataFactory);
            writer.write(doc, indent);
        }
        
        void XMLHelperImpl::save(
            DataObjectPtr dataObject,
            const char* rootElementURI,
            const char* rootElementName,
            const char* xmlFile,
            int indent)
        {
            save(createDocument(dataObject,rootElementURI, rootElementName), xmlFile,
                indent);
        }
        void XMLHelperImpl::save(
            DataObjectPtr dataObject,
            const SDOString& rootElementURI,
            const SDOString& rootElementName,
            const SDOString& xmlFile,
            int indent)
        {
            save(createDocument(dataObject,rootElementURI, rootElementName), xmlFile,
                indent);
        }
        
        
        // Serializes the datagraph to a stream
        void XMLHelperImpl::save(XMLDocumentPtr doc, std::ostream& outXml,
            int indent)
        {
            SDOXMLStreamWriter writer(outXml, dataFactory);
            writer.write(doc, indent);                
        }

        void XMLHelperImpl::save(
            DataObjectPtr dataObject,
            const char* rootElementURI,
            const char* rootElementName,
            std::ostream& outXml,
            int indent )
        {
            save(createDocument(dataObject,rootElementURI, rootElementName), outXml, indent);
        }
        void XMLHelperImpl::save(
            DataObjectPtr dataObject,
            const SDOString& rootElementURI,
            const SDOString& rootElementName,
            std::ostream& outXml,
            int indent )
        {
            save(createDocument(dataObject,rootElementURI, rootElementName), outXml, indent);
        }
        
        // Serializes the datagraph to a std::string
        char* XMLHelperImpl::save(XMLDocumentPtr doc,
            int indent)
        {
            SDOXMLBufferWriter writer(dataFactory);
            writer.write(doc, indent);
            SDOXMLString ret = writer.getBuffer();
            char* retString = new char[strlen(ret) +1];
            strcpy(retString, ret);
            return retString;
        }
        char* XMLHelperImpl::save(
            DataObjectPtr dataObject,
            const char* rootElementURI,
            const char* rootElementName,
            int indent)
        {
            return save(createDocument(dataObject,rootElementURI, rootElementName),
                indent);
        }
        char* XMLHelperImpl::save(
            DataObjectPtr dataObject,
            const SDOString& rootElementURI,
            const SDOString& rootElementName,
            int indent)
        {
            return save(createDocument(dataObject,rootElementURI, rootElementName),
                indent);
        }

        unsigned int XMLHelperImpl::getErrorCount() const
        {
            return parseErrors.size();
        }


        const char* XMLHelperImpl::getErrorMessage(unsigned int errnum) const
        {
            if (errnum >= 0 && errnum < parseErrors.size())
            {
                return parseErrors[errnum];
            }
            return 0;
        }

        void XMLHelperImpl::setError(const char* message)
        {
            if (message == 0) return;
            char * m = new char[strlen(message) + 1];
            strcpy(m,message);
            m[strlen(message)] = 0;
            parseErrors.push_back(m);
        }

        void XMLHelperImpl::clearErrors()
        {
            while (!parseErrors.empty()) 
            {
                if (*parseErrors.begin() != 0)
                {
                    delete[] (char*)(*parseErrors.begin());
                }
                parseErrors.erase(parseErrors.begin());
            }
        }
        
    } // End - namespace sdo
} // End - namespace commonj
