<template>
  <q-page class="flex flex-center">
    <div class="q-pa-md q-gutter-sm my-font">
      <h2 class="text-h4">LED Status: {{ ledState }}</h2>
      <q-btn
        v-on:click="TurnOnLED"
        color="cyan-10"
        class="my-font"
        label="Turn ON LED"
      />
      <q-btn
        v-on:click="TurnOffLED"
        color="pink-8"
        glossy
        label="Turn OFF LED"
      />
    </div>
  </q-page>
</template>

<script>
import axios from "axios";
import { defineComponent } from "vue";

export default defineComponent({
  name: "PageIndex",
  data() {
    return {
      ledState: "OFF",
    };
  },
  methods: {
    TurnOnLED() {
      this.ledState = 1;
      axios
        .post("/api/v1/light/brightness", { ledState: this.ledState })
        .then((data) => {
          console.log(data);
        });
    },
    TurnOffLED() {
      this.ledState = 0;
      axios
        .post("/api/v1/light/brightness", { ledState: this.ledState })
        .then((data) => {
          console.log(data);
        });
    },
  },
});
</script>
