#include <cloud.h>
#include <../daemon/object.h>
#include <../daemon/buffer.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
	struct cld_object *object = cld_object_create("account");
	
	struct cld_object *user = cld_object_create("user");
	cld_object_set_field(user, "username", cld_object_create_string("fabianschuiki"));
	cld_object_set_field(user, "password", cld_object_create_string("HeLo.Aweorld"));
	cld_object_set_field(object, "dschingsta", user);
	
	cld_object_print(object);
	
	struct cld_buffer *buffer = cld_object_serialize(object);
	if (buffer != NULL) {
		FILE * f = fopen("object.serialized", "w");
		fwrite(buffer->data, buffer->length, 1, f);
		fclose(f);
	} else {
		fprintf(stderr, "serializing object failed\n");
	}
	
	cld_object_destroy(object);
	
	if (buffer != NULL) {
		object = cld_object_unserialize(buffer->data, buffer->length);
		if (object == NULL) {
			fprintf(stderr, "unserializing object failed\n");
		} else {
			cld_object_print(object);
			cld_object_destroy(object);
		}
		cld_buffer_destroy(buffer);
	}
	return 0;
}

