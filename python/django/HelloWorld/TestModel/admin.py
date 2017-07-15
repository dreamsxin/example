from django.contrib import admin
from TestModel.models import Test,Contact,Tag

# Register your models here.
admin.site.register(Test)
admin.site.register([Contact, Tag])