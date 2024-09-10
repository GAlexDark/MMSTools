/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2020 Christian Hohnstaedt.
 *
 * All rights reserved.
 */

#ifndef __BIOBYTEARRAY_H
#define __BIOBYTEARRAY_H

#include <QByteArray>
#include <QString>
#include <openssl/bio.h>

class BioByteArray
{
  protected:
	BIO *read_write{};
	BIO *read_only{};
	QByteArray store{};

	void set(const QByteArray &qba);
	void add(const QByteArray &qba);
	void biowrite(const QByteArray &qba);
	void cleanse_and_free(BIO *bio);

  public:
    explicit BioByteArray(const QByteArray &qba) : store(qba) { }
    explicit BioByteArray(const BioByteArray &bba) : store(bba.byteArray()) { }
    BioByteArray() = default;
	~BioByteArray();
	int size() const;
	BIO *bio();
	BIO *ro();
	QByteArray byteArray() const;
	QString qstring() const;
	operator BIO*();
	operator QByteArray();
	BioByteArray &operator = (const BioByteArray &other);
	BioByteArray &operator = (const QByteArray &qba);
	BioByteArray &operator += (const BioByteArray &other);
	BioByteArray &operator += (const QByteArray &qba);
};
#endif
