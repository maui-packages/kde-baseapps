/* This file is part of the KDE project
   Copyright (C) 1998, 1999 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <unistd.h>

#include "kfileicon.h"

#include <kmimetypes.h>
#include <kpixmapcache.h>
#include <klocale.h>

KFileIcon::KFileIcon( UDSEntry& _entry, KURL& _url ) :
  m_entry( _entry ), 
  m_url( _url ), 
  m_bMarked( false ),
  m_bIsLocalURL( _url.isLocalFile() )
{
  init();
}

void KFileIcon::init()
{
  mode_t mode = 0;
  UDSEntry::iterator it = m_entry.begin();
  for( ; it != m_entry.end(); it++ )
    if ( it->m_uds == UDS_FILE_TYPE )
      mode = (mode_t)it->m_long;

  m_pMimeType = KMimeType::findByURL( m_url, mode, m_bIsLocalURL );
  assert (m_pMimeType);
}

bool KFileIcon::acceptsDrops( QStrList& /* _formats */ )
{
  if ( strcmp( "inode/directory", m_pMimeType->mimeType() ) == 0 )
    return true;

  if ( !m_bIsLocalURL )
    return false;

  if ( strcmp( "application/x-desktop", m_pMimeType->mimeType() ) == 0 )
    return true;

  // Executable, shell script ... ?
  if ( access( m_url.path(), X_OK ) == 0 )
    return true;

  return false;
}

QString KFileIcon::getStatusBarInfo()
{
  QString comment = m_pMimeType->comment( m_url, false );
  QString text;
  QString linkDest;

  long size   = 0;
  mode_t mode = 0;

  UDSEntry::iterator it = m_entry.begin();
  for( ; it != m_entry.end(); it++ )
  {
    if ( it->m_uds == UDS_SIZE )
      size = it->m_long;
    else if ( it->m_uds == UDS_FILE_TYPE )
      mode = (mode_t)it->m_long;
    else if ( it->m_uds == UDS_LINK_DEST )
      linkDest = it->m_str.c_str();
    else if ( it->m_uds == UDS_NAME )
      text = it->m_str.c_str();
  }

  QString text2 = text.copy();

  if ( m_url.isLocalFile() )
  {
    if ( S_ISLNK( mode ) )
    {
      QString tmp;
      if ( comment.isEmpty() )
	tmp = i18n ( "Symbolic Link" );
      else
        tmp = i18n("%1 (Link)").arg(comment);
      text += "->";
      text += linkDest;
      text += "  ";
      text += tmp;
    }
    else if ( S_ISREG( mode ) )
    {
      if (size < 1024)
        text = QString("%1 (%2 %3)").arg(text2).arg((long) size).arg(i18n("bytes"));
      else
      {
	float d = (float) size/1024.0;
        text = QString("%1 (%2 K)").arg(text2).arg(d, 0, 'f', 2); // was %.2f
      }
      text += "  ";
      text += comment;
    }
    else if ( S_ISDIR( mode ) )
    {
      text += "/  ";
      text += comment;
    }
    else
    {
      text += "  ";
      text += comment;
    }	
    return text;
  }
  else
    return m_url.decodedURL();
}

